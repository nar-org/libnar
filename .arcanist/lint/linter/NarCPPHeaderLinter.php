<?php

final class NarCPPHeaderLinter extends ArcanistLinter {

  const LINT_NO_HEADER         = 1;
  const LINT_NO_DOXYGEN_HEADER = 2;
  const LINT_EXPECTING_MULTILINE_COMMENT = 3;
  const LINT_EXPECTING_DOXYGEN_COMMENT = 4;
  const LINT_NO_LICENSE        = 5;
  const LINT_UNEXPECTED_LINE   = 6;
  const LINT_MISSING_TAG       = 7;

  private $author = "Nicolas DI PRIMA <nicolas@di-prima.fr>";
  private $owner  = "Nicolas DI PRIMA";

  /**
   * This is the function to use to generate the default license for a given
   * file. */
  private function getDefaultLicense($path) {
    return '/**
 * @file    ' . $path . '
 * @authors ' . $this->author . '
 * @date    ' . date('Y-m-d') . '
 *
 * @copyright
 *    Copyright (C) ' . date('Y') . ', ' . $this->owner . '. All rights reserved.
 *
 *    This software may be modified and distributed under the terms
 *    of the BSD license. See the LICENSE file for details.
 */';
  }

  public function getInfoName() { return pht('NAR standard C++ header and copyright.'); }
  public function getInfoDescription() {
    return pht("Enforce a standardized Header for every C++ files.");
  }
  public function getLinterName() { return pht('NarCPPHeaderLinter'); }
  public function getLinterPriority() { return 0.8; }
  public function getLinterConfigurationName() { return pht("nar-c++-header"); }
  public function getLinterConfigurationOptions() {
    $options = array(
      'nar.owner' => array(
        'type' => 'string',
        'help' => pht('owner information for copyright purpose'),
      ),
      'nar.author' => array(
        'type' => 'string',
        'help' => pht('author information for information purpose'),
      ),
    );
    return $options + parent::getLinterConfigurationOptions();
  }
  public function setLinterConfigurationValue($key, $value) {
    switch ($key) {
    case 'nar.owner':
      $this->owner = $value;
      return;
    case 'nar.author':
      $this->author = $value;
      return;
    }
    return parent::setLinterConfigurationValue($key, $value);
  }
  public function getLintSeverityMap() {
    return array(
      self::LINT_NO_LICENSE      => ArcanistLintSeverity::SEVERITY_AUTOFIX,
      self::LINT_UNEXPECTED_LINE => ArcanistLintSeverity::SEVERITY_AUTOFIX,
      self::LINT_MISSING_TAG     => ArcanistLintSeverity::SEVERITY_AUTOFIX,
      self::LINT_EXPECTING_DOXYGEN_COMMENT => ArcanistLintSeverity::SEVERITY_AUTOFIX,
    );
  }
  public function getLintNameMap() {
    return array (
      self::LINT_NO_LICENSE      => pht('No license'),
      self::LINT_UNEXPECTED_LINE => pht('wrong header line'),
      self::LINT_MISSING_TAG     => pht('Missing tag'),
      self::LINT_EXPECTING_DOXYGEN_COMMENT => pht('Doxygen comment required'),
    );
  }
  public function lintPath($path) {
    if (!strlen($this->getData($path))) {
      return; // if the file is empty, then nothing to do
    }

    $lines = explode("\n", $this->getData($path));
    if (0 === count($lines)) {
      $this->raiseLintAtLine
        ( 1
        , 10
        , self::LINE_NO_LICENSE
        , pht("Header must conform to standard doxygen with copyright information.")
        , $lines[0]
        , $this->getDefaultLicense($path)
        );
      return;
    }

    if (!$this->checkLineStartDoxygenHeader(0, $lines[0])) {
      return;
    }
    $offset_start = 3;

    $endOfComments = array_keys($lines, " */");
    if (count($endOfComments) < 0) {
      // error ??
    }
    $endOfComment = $endOfComments[0] - 1; // we don't want to check the last

    $header_lines = array_slice($lines, 0, $endOfComment);

    $this->checkDoxygenContent( 0 , $offset_start, $header_lines
                              , "file", "    ", "\s+" . $path
                              , $path
                              );
    $this->checkDoxygenContent( 0 , $offset_start, $header_lines
                              , "authors", " ", "\s+.+"
                              , $this->author
                              );
    $this->checkDoxygenContent( 0 , $offset_start, $header_lines
                              , "date", "    ", "\s+\d{4}-\d{2}-\d{2}"
                              , date("Y-m-d")
                              );
  }

  private function checkDoxygenContent( $lines_start_idx, $offset_start_idx, $lines
                                      , $doxytag, $doxyspace, $doxyregex
                                      , $doxyreplace)
  {
    $line_info = 0;
    foreach ($lines as $line_idx => $line) {
      if (strpos($line, "@".$doxytag) !== false) {
        $line_info = $line_idx;
        break;
      }
    }

    if ($line_info === 0) {
      $this->raiseLintAtOffset
        ( $offset_start_idx + 1
        , self::LINT_MISSING_TAG
        , pht("Standard header contains tag: " . $doxytag)
        , ""
        , " * @" . $doxytag . $doxyspace . $doxyreplace . "\n"
        );
      return;
    }

    if (1 !== preg_match("%^ \* @" . $doxytag . $doxyregex . "$%", $lines[$line_info])) {
      $this->raiseLintAtLine
        ( $lines_start_idx + $line_info + 1
        , 1
        , self::LINT_MISSING_TAG
        , pht("Standard header contains tag: " . $doxytag)
        , $lines[$line_info]
        , " * @" . $doxytag . $doxyspace . $doxyreplace
        );
    }
  }
//    $this->checkOrReplace(5, $lines[5], "%^ \* @copyright$%", " * @copyright");
//    $this->checkOrReplace(6, $lines[6]
//      , "%^ \*    Copyright \(c\) \d{4}, .+\. All rights reserved.$%"
//      ,    " *    Copyright (c) " . date("Y") . ", " . $this->owner . ". All rights reserved.");
//    $this->checkOrReplace(8, $lines[8]
//      , "%^ \*    This software may be modified and distributed under the terms$%"
//      ,   " *    This software may be modified and distributed under the terms");
//    $this->checkOrReplace(9, $lines[9]
//      , "%^ \*    of the BSD license. See the LICENSE file for details.$%"
//      ,   " *    of the BSD license. See the LICENSE file for details.");

  private function checkLineStartDoxygenHeader($line_idx, $line) {
    if ($line === "//" || $line === "///") { // TODO no autofix
      $this->raiseLintAtLine
        ( $line_idx + 1
        , 1
        , self::LINT_EXPECTING_MULTILINE_COMMENT
        , pht("Standard header is a multiline Doxygen comment")
        , $line
        );
      return false;
    }

    if ($line === "/*") { // TODO autofix
      $this->raiseLintAtLine
        ( $line_idx + 1
        , 1
        , self::LINT_EXPECTING_DOXYGEN_COMMENT
        , pht("Standard header is a multiline Doxygen comment")
        , $line
        , "/**"
        );
      return false;
    }

    if ($line !== "/**") {
      $this->raiseLintAtLine
        ( $line_idx
        , 1
        , self::LINT_NO_DOXYGEN_HEADER
        , pht("Standard header is a multiline Doxygen comment")
        , ""
        , $this->getDefaultLicense($path)
        );
      return false;
    }
    return true;
  }
}

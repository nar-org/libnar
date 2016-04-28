<?php

final class NarLintEngine extends ArcanistLintEngine {
  public function buildLinters() {
    $paths = $this->getPaths();

    $text_linter = new ArcanistTextLinter();
    $filename_linter = new ArcanistFilenameLinter();
    $cpp_linter1 = new ArcanistCppcheckLinter();
    $cpp_linter2 = new ArcanistCpplintLinter();

    foreach ($paths as $key => $path) {
      if (!$this->pathExists($path)) {
        unset($paths[$key]);
      }
    }

    foreach ($paths as $path) {
      if (preg_match('@^.arcanist/\..*@', $path)) {
        continue;
      }
      if (preg_match('/\.(cpp|hpp)$/', $path)) {
        $cpp_linter1->addPath($path);
        $cpp_linter2->addPath($path);
      }
      $text_linter->addPath($path);
      $filename_linter->addPath($path);
    }
    return array(
      $text_linter,
      $filename_linter,
      $cpp_linter1,
      $cpp_linter2,
    );
  }
}

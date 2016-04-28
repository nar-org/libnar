/**
 * @file    app/src/Archive.cpp
 * @authors Nicolas DI PRIMA <nicolas@di-prima.fr>
 * @date    2016-04-11
 *
 * @Copyright Nicolas DI PRIMA
 */

#include "Archive.hpp"

#include <iostream>
#include <string>
#include <vector>

Archive::Archive()
  : filepath_()
  , extract_(false)
  , list_(false)
{
}

void Archive::set( po::options_description& desc
                 , po::positional_options_description& pos) {
  desc.add_options()
    ( "archive,a"
    , po::value<std::string>()
    , "create or append file or directory in the given archive"
    )
    ( "extract,x"
    , "extract the given archive (will append otherwise)"
    )
    ( "list,l"
    , "list the content from the given archive"
    )
    ( "input-file"
    , po::value<std::vector<std::string> >()
    , "one or more file path"
    )
    ;
  pos.add("input-file", -1);
}

void Archive::config(po::variables_map const& vm) {
  if (vm.count("archive")) {
    filepath_ = vm["archive"].as<std::string>();
  } else {
    std::cerr
      << "You must provide a file to work on. This application will"
      << std::endl
      << "work only with files (there is no guaranty to work with other"
      << std::endl
      << "file descriptors. see --help for more information."
      << std::endl;
    throw option_error();
  }
  if (vm.count("input-file")) {
    inputs_ = vm["input-file"].as<std::vector<std::string> >();
  }
  extract_ = vm.count("extract");
  list_    = vm.count("list");

  if (list_) {
    if (extract_) {
      std::cerr << "You cannot extract and list an archive" << std::endl;
      throw option_error();
    }
    if (!inputs_.empty()) {
      std::cerr
        << "No need for inputs PATH when listing an archive" << std::endl;
      throw option_error();
    }
  } else if (extract_) {
    if (inputs_.size() > 1) {
      std::cerr
        << "You can only extract the given archive in one directory"
        << std::endl;
      throw option_error();
    }
  } else {
    if (inputs_.size() == 0) {
      std::cerr
        << "You must give one or more file/directory to append in the"
        << std::endl
        << "archive. see --help for more informaiton." << std::endl;
      throw option_error();
    }
  }
}

bool               Archive::extract()  const { return extract_; }
bool               Archive::list()     const { return list_; }
std::string const& Archive::filepath() const { return filepath_; }
Paths const&       Archive::paths()    const { return inputs_; }

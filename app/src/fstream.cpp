/**
 * @file    app/src/fstream.cpp
 * @authors Nicolas DI PRIMA <nicolas@di-prima.fr>
 * @date    2016-04-11
 *
 * @Copyright Nicolas DI PRIMA
 */

#include "nar/fstream.hpp"

#include <string>
#include <fstream>

#include "boost/filesystem.hpp"

#include "nar/roundup.hpp"

namespace fs = boost::filesystem;

namespace nar {

ofstream::ofstream(char const* f)        : ofs_() { open(f); }
ofstream::ofstream(std::string const& f) : ofs_() { open(f); }

void ofstream::open(char const* f) {
  fs::path p(f);
  bool add_header = false;
  if (!fs::exists(p)) {
    add_header = true;;
  } else {
    if (!fs::is_regular_file(p)) {
      // TODO(NicolasP): ERROR! not a regular file
    } else {
      std::ifstream ifs(f, std::ios_base::in
                         | std::ios_base::binary
                       );
      nar::header::narh header;
      ifs >> header;
    }
  }
  ofs_.open( f
           , std::ios_base::out     // we are writing
           | std::ios_base::binary  // in binary mode
           | std::ios_base::ate     // at the end of the file
           | std::ios_base::app     // we append
           );
  if (add_header) {
    ofs_ << nar::header::narh();
  }
}
void ofstream::open(std::string const& f) { this->open(f.c_str()); }
bool ofstream::is_open() const { return ofs_.is_open(); }
void ofstream::close() { ofs_.close(); }

ofstream& ofstream::flush() { ofs_.flush(); return *this; }

ofstream& ofstream::padding(std::uint64_t p) {
  while (p--) { ofs_ << std::ends; }
  return *this;
}
ofstream& ofstream::append(std::string const& path) {
  fs::path p(path);
  if (!fs::exists(p)) {
    // TODO(NicolasDP): error to throw
  }

  if (fs::is_regular_file(p)) {
    nar::header::file file;
    file.flags = 0;  // TODO(NicolasDP): need setting
    file.length_1 = path.length();

    std::ifstream ifs(path, std::ios_base::in | std::ios_base::binary);
    ifs.seekg (0, ifs.end);
    file.length_2 = ifs.tellg();
    ifs.seekg (0, ifs.beg);

    ofs_ << file;
    ofs_ << path;        padding(PADDING64(file.length_1));
    ofs_ << ifs.rdbuf(); padding(PADDING64(file.length_2));
    flush();
  } else if (fs::is_directory(p)) {
    for (fs::directory_entry& x : fs::directory_iterator(p)) {
      append(x.path().native());
    }
  }

  return *this;
}

} /* ! namespace nar */

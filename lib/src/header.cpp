/**
 * @file    lib/src/header.cpp
 * @authors Nicolas DI PRIMA <nicolas@di-prima.fr>
 * @date    2016-04-14
 *
 * @copyright
 *    Copyright (C) 2016, Nicolas DI PRIMA. All rights reserved.
 *
 *    This software may be modified and distributed under the terms
 *    of the BSD license. See the LICENSE file for details.
 *
 * @brief explicit instantiation of nar::header
 */

#include "nar/header.hpp"

namespace nar {

namespace header {

namespace generic {

header::header( std::uint64_t m
              , std::uint64_t f
              , std::uint64_t l1
              , std::uint64_t l2
              )
  : magic(m)
  , flags(f)
  , length_1(l1)
  , length_2(l2)
{
}

bool header::operator== (header const& e) const noexcept
{
  return this->magic == e.magic
      && this->flags == e.flags
      && this->length_1 == e.length_1
      && this->length_2 == e.length_2;
}

void header::set_compression_1() noexcept { flags |=  flags_compression_1; }
void header::clr_compression_1() noexcept { flags &= ~flags_compression_1; }
void header::set_compression_2() noexcept { flags |=  flags_compression_2; }
void header::clr_compression_2() noexcept { flags &= ~flags_compression_2; }

bool header::compression_1() const noexcept {
  return flags & flags_compression_1;
}
bool header::compression_2() const noexcept {
  return flags & flags_compression_2;
}
}  /* ! namespace generic */

template struct basic_header< known_magic::initiate<std::uint64_t>
                            , default_flags_initiate
                            >;
template struct basic_header<known_magic::file<std::uint64_t> >;

}  /* ! namespace header */

std::uint16_t version(header::initiate const& t) {
  return (t.flags >> 46) & 0x0000FFFF;
}
void version(header::initiate& t, std::uint16_t const& v) {
  t.flags &= 0x0000FFFFFFFF;
  t.flags |= (static_cast<std::uint64_t>(v) << 46);
}

bool executable(header::file const& t) {
  return t.flags & flags_file_executable;
}
void executable(header::file& t, bool const v) {
  if (v) { t.flags |=  flags_file_executable;
  } else { t.flags &= ~flags_file_executable;
  }
}

} /* ! namespace nar */

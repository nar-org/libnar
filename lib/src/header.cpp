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

void header::set_compression_1() noexcept { flags |= 1 << 0; }
void header::clr_compression_1() noexcept { flags ^= 1 << 0; }
void header::set_compression_2() noexcept { flags |= 1 << 1; }
void header::clr_compression_2() noexcept { flags ^= 1 << 1; }

bool header::compression_1() const noexcept { return flags & (1 << 0); }
bool header::compression_2() const noexcept { return flags & (1 << 1); }

}  /* ! namespace generic */

}  /* ! namespace header */

template struct header::basic_header<nar::known_magic::narh<std::uint64_t> >;
template struct header::basic_header<nar::known_magic::file<std::uint64_t> >;

} /* ! namespace nar */

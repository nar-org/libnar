/**
 * @file    lib/include/nar/generic/header.hpp
 * @authors Nicolas DI PRIMA <nicolas@di-prima.fr>
 * @date    2016-04-14
 *
 * @copyright
 *    Copyright (C) 2016, Nicolas DI PRIMA. All rights reserved.
 *
 *    This software may be modified and distributed under the terms
 *    of the BSD license. See the LICENSE file for details.
 *
 * @brief generic definition of a NAR header
 */
#ifndef LIB_INCLUDE_NAR_GENERIC_HEADER_HPP_
# define LIB_INCLUDE_NAR_GENERIC_HEADER_HPP_

# include <cstdint>
# include <type_traits>

# include "nar/standard.hpp"

namespace nar {
namespace generic {

struct header {
  std::uint64_t magic;
  std::uint64_t version;
  std::uint64_t cipher;
  std::uint64_t compression;
  std::uint64_t signature_position;
  std::uint64_t index_position;
  std::uint64_t unused_1;
  std::uint64_t unused_2;

  explicit
  header( std::uint64_t m  = 0
        , std::uint64_t v  = current_version
        , std::uint64_t cr = 0
        , std::uint64_t cn = 0
        , std::uint64_t s  = 0
        , std::uint64_t i  = 0
        , std::uint64_t u1 = 0
        , std::uint64_t u2 = 0
        )
    : magic(m)
    , version(v)
    , cipher(cr)
    , compression(cn)
    , signature_position(s)
    , index_position(i)
    , unused_1(u1)
    , unused_2(u2)
  {
  }

  header(header const&)            = default;
  header(header &&)                = default;
  header& operator=(header const&) = default;
  header& operator=(header &&)     = default;

  ~header()                        = default;

  bool operator== (header const& e2) const {
    return this->magic == e2.magic
        && this->version == e2.version
        && this->cipher == e2.cipher
        && this->compression == e2.compression
        && this->signature_position == e2.signature_position
        && this->index_position == e2.index_position
        && this->unused_1 == e2.unused_1
        && this->unused_2 == e2.unused_2;
  }
} __attribute__((packed));


static_assert( 64 == sizeof(header)
             , "Nar Specification said that a Nar Header is 64bytes."
             );
static_assert( std::is_trivially_copyable<header>::value
             , "Oops"
             );

} /* ! namespace generic */
} /* ! namespace nar */

#endif /* ! LIB_INCLUDE_NAR_GENERIC_HEADER_HPP_ */

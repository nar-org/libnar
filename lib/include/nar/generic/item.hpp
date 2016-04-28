/**
 * @file    lib/include/nar/generic/item.hpp
 * @authors Nicolas DI PRIMA <nicolas@di-prima.fr>
 * @date    2016-04-15
 *
 * @copyright
 *    Copyright (C) 2016, Nicolas DI PRIMA. All rights reserved.
 *
 *    This software may be modified and distributed under the terms
 *    of the BSD license. See the LICENSE file for details.
 *
 * @brief generic definition of a NAR Item header
 */
#ifndef LIB_INCLUDE_NAR_GENERIC_ITEM_HPP_
# define LIB_INCLUDE_NAR_GENERIC_ITEM_HPP_

# include <cstdint>
# include <type_traits>

# include "nar/standard.hpp"

namespace nar {
namespace generic {

struct item {
  std::uint64_t magic;
  std::uint64_t flags;
  std::uint64_t length_1;
  std::uint64_t length_2;

  explicit
  item( std::uint64_t m  = 0
      , std::uint64_t f  = 0
      , std::uint64_t l1 = 0
      , std::uint64_t l2 = 0
      )
    : magic(m)
    , flags(f)
    , length_1(l1)
    , length_2(l2)
  {
  }

  item(item const&)            = default;
  item(item &&)                = default;
  item& operator=(item const&) = default;
  item& operator=(item &&)     = default;

  ~item()                      = default;

  bool operator== (item const& e2) const {
    return this->magic == e2.magic
        && this->flags == e2.flags
        && this->length_1 == e2.length_1
        && this->length_2 == e2.length_2;
  }
} __attribute__((packed));


static_assert( 32 == sizeof(item)
             , "Nar Specification said that a Nar Item is 32bytes."
             );
static_assert( std::is_trivially_copyable<item>::value
             , "Oops"
             );

} /* ! namespace generic */
} /* ! namespace nar */

#endif /** ! LIB_INCLUDE_NAR_GENERIC_ITEM_HPP_  */

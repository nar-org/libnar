/**
 * @file    lib/include/nar/item.hpp
 * @authors Nicolas DI PRIMA <nicolas@di-prima.fr>
 * @date    2016-04-16
 *
 * @copyright
 *    Copyright (C) 2016, Nicolas DI PRIMA. All rights reserved.
 *
 *    This software may be modified and distributed under the terms
 *    of the BSD license. See the LICENSE file for details.
 *
 * @brief basic_item definition and properties
 *
 * # nar::basic_item
 *
 * Generic definition for every basic item. Use this templated structure to
 * define your own NAR item header with the type deduction based on the MAGIC
 * given in the type parameter. The assertion over the MAGIC number is made
 * at construction time or at assignment time (copy from a generic one).
 *
 * # Example
 *
 * @code{cpp}
 * constexpr std::uint64_t magic = 0xbeefbeef;
 * using beef = nar::basic_item<magic>;
 *
 * int main() {
 *   {
 *     beef b;
 *     std::stringstream ss;
 *     ss << 0xbeefbeef
 *        <<"\0\0\0\0\0\0\0\0"
 *        <<"\0\0\0\0\0\0\0\0"
 *        <<"\0\0\0\0\0\0\0\0";
 *     ss >> b; // value assigned
 *   }
 *   {
 *     beef b;
 *     std::stringstream ss;
 *     ss << 0xdeaddead
 *        <<"\0\0\0\0\0\0\0\0"
 *        <<"\0\0\0\0\0\0\0\0"
 *        <<"\0\0\0\0\0\0\0\0";
 *     ss >> b; // throw exception
 *   }
 * }
 * @encode
 *
 * # Standard Item Header
 *
 * ## nar::file
 *
 * This item header is defined in the NAR specification.
 * The default magic is defined in nar::known_magic::file. It specifies the
 * item is a file. See specification to know more about it.
 */
#ifndef LIB_INCLUDE_NAR_ITEM_HPP_
# define LIB_INCLUDE_NAR_ITEM_HPP_

# include <cstdint>

# include <type_traits>

# include "nar/error.hpp"
# include "nar/standard.hpp"
# include "nar/generic/item.hpp"

namespace nar {

template<class C>
struct is_basic_item : std::is_base_of<generic::item, C> { };

template<std::uint64_t MAGIC>
struct basic_item : generic::item {
  basic_item()  : generic::item(MAGIC) { }

  basic_item(generic::item const& h) : generic::item(h) {
    NAR_ASSERT_MAGIC(MAGIC, h.magic);
  }
  basic_item(generic::item&& h) : generic::item(std::move(h)) {
    NAR_ASSERT_MAGIC(MAGIC, h.magic);
  }

  basic_item<MAGIC>& operator= (generic::item const& h) {
    NAR_ASSERT_MAGIC(MAGIC, h.magic);
    generic::item::operator= (h);
    return *this;
  }
  basic_item<MAGIC>& operator= (generic::item&& h) {
    NAR_ASSERT_MAGIC(MAGIC, h.magic);
    generic::item::operator= (std::move(h));
    return *this;
  }

  bool operator== (basic_item<MAGIC> const& e2) const {
    return generic::item::operator==(e2);
  }
};

extern template struct basic_item<known_magic::file<std::uint64_t> >;
using file = basic_item<known_magic::file<std::uint64_t> >;

} /* ! namespace nar */

#endif /** ! LIB_INCLUDE_NAR_ITEM_HPP_  */

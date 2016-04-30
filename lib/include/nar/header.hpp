/**
 * @file    lib/include/nar/header.hpp
 * @authors Nicolas DI PRIMA <nicolas@di-prima.fr>
 * @date    2016-04-17
 *
 * @copyright
 *    Copyright (c) 2016, Nicolas DI PRIMA. All rights reserved.
 *    This software may be modified and distributed under the terms
 *    of the BSD license. See the LICENSE file for details.
 *
 * @brief basic_header definition and properties
 *
 * # nar::basic_header
 *
 * Generic definition of the NAR header. Use this templated structure to
 * define your own NAR header with the type deduction based on the MAGIC
 * given in the type parameter. The assertion over the MAGIC number is made
 * at construction time or at assignment time (copy from a generic one).
 *
 * # Example
 *
 * @code{cpp}
 * constexpr std::uint64_t magic = 0xbeefbeef;
 * using beef = nar::basic_header<magic>;
 * @encode
 *
 * # Standard Item Header
 *
 * >>> nar::header
 *
 * This header is defined in the NAR specification.
 * The default magic is defined in nar::known_magic::header.
 *
 * Basically you do not need to change it unless you want to redefine a new
 * type of NAR.
 */
#ifndef LIB_INCLUDE_NAR_HEADER_HPP_
# define LIB_INCLUDE_NAR_HEADER_HPP_

# include <cstdint>

# include <type_traits>

# include "nar/error.hpp"
# include "nar/standard.hpp"

namespace nar {
namespace header {
namespace generic {

struct header {
  std::uint64_t magic;
  std::uint64_t flags;
  std::uint64_t length_1;
  std::uint64_t length_2;

  header( std::uint64_t m  = 0
        , std::uint64_t f  = 0
        , std::uint64_t l1 = 0
        , std::uint64_t l2 = 0
        );

  header(header const&)            = default;
  header(header &&)                = default;
  header& operator=(header const&) = default;
  header& operator=(header &&)     = default;
  ~header()                        = default;

  bool operator == (header const&) const noexcept;

  void set_compression_1() noexcept;
  void clr_compression_1() noexcept;
  void set_compression_2() noexcept;
  void clr_compression_2() noexcept;
  bool compression_1() const noexcept;
  bool compression_2() const noexcept;
};

} /* ! namespace generic */

template<std::uint64_t MAGIC>
struct basic_header : generic::header {
  basic_header() : generic::header(MAGIC) { }

  explicit
  basic_header(generic::header const& h) : generic::header(h) {
    NAR_ASSERT_MAGIC(MAGIC, magic);
  }

  basic_header<MAGIC>& operator= (generic::header const& h) {
    NAR_ASSERT_MAGIC(MAGIC, h.magic);
    generic::header::operator= (h);
    return *this;
  }

  bool operator== (basic_header<MAGIC> const& e2) const {
    return generic::header::operator== (e2);
  }
};

extern template struct basic_header<known_magic::narh<std::uint64_t> >;
using narh = basic_header<known_magic::narh<std::uint64_t> >;
extern template struct basic_header<known_magic::file<std::uint64_t> >;
using file = basic_header<known_magic::file<std::uint64_t> >;

} /* ! namespace header */
/**
 * @type_traits template<class C> struct is_basic_header<T>
 * @brief compile type checker that the given class is of type header.
 */
template<class C>
struct is_header : std::is_base_of<header::generic::header, C> { };

} /* ! namespace nar */
#endif /** ! LIB_INCLUDE_NAR_HEADER_HPP_  */

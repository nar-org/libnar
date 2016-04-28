/**
 * @file    lib/include/nar/roundup.hpp
 * @authors Nicolas DI PRIMA <nicolas@di-prima.fr>
 * @date    2016-04-11
 *
 * @copyright
 *    Copyright (C) 2016, Nicolas DI PRIMA. All rights reserved.
 *
 *    This software may be modified and distributed under the terms
 *    of the BSD license. See the LICENSE file for details.
 *
 * @brief roundup and padding implementation
 */
#ifndef LIB_INCLUDE_NAR_ROUNDUP_HPP_
# define LIB_INCLUDE_NAR_ROUNDUP_HPP_

# include <cstdint>

# include <type_traits>

namespace nar {

template<typename T, T ROUND>
typename std::enable_if< std::is_arithmetic<T>::value
                       , T
                       >::type
ROUND_UP(T const& t)
{
  static_assert(ROUND > 0, "oops");
  return ((t - 1) / ROUND + 1) * ROUND;
}
template<typename T, T ROUND>
typename std::enable_if< std::is_arithmetic<T>::value
                       , T
                       >::type
PADDING(T const& t)
{
  return ROUND_UP<T, ROUND>(t) - t;
}

std::uint64_t ROUND_UP64(std::uint64_t const&);
std::uint64_t PADDING64(std::uint64_t const&);

} /* ! namespace nar */

#endif /** ! LIB_INCLUDE_NAR_ROUNDUP_HPP_  */

/**
 * @file    lib/src/roundup.cpp
 * @authors Nicolas DI PRIMA <nicolas@di-prima.fr>
 * @date    2016-04-13
 *
 * @copyright
 *    Copyright (C) 2016, Nicolas DI PRIMA. All rights reserved.
 *
 *    This software may be modified and distributed under the terms
 *    of the BSD license. See the LICENSE file for details.
 *
 * @brief specialised implementation for roundup and padding
 */

#include "nar/roundup.hpp"

namespace nar {

std::uint64_t ROUND_UP64(std::uint64_t const& t) {
  return ROUND_UP<std::uint64_t, sizeof(std::uint64_t)>(t);
}
std::uint64_t PADDING64(std::uint64_t const& t) {
  return PADDING<std::uint64_t, sizeof(std::uint64_t)>(t);
}

} /* ! namespace nar */

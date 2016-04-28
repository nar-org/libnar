/**
 * @file    lib/src/item.cpp
 * @authors Nicolas DI PRIMA <nicolas@di-prima.fr>
 * @date    2016-04-15
 *
 * @copyright
 *    Copyright (C) 2016, Nicolas DI PRIMA. All rights reserved.
 *
 *    This software may be modified and distributed under the terms
 *    of the BSD license. See the LICENSE file for details.
 *
 * @brief explicit instantiation of nar::file
 */

#include <type_traits>

#include "nar/item.hpp"
#include "nar/standard.hpp"

namespace nar {

template struct basic_item<known_magic::file<std::uint64_t> >;

static_assert( std::is_trivially_copyable<file>::value
             , "Oops"
             );

} /* ! namespace nar */

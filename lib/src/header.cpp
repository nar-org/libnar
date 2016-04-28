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

template struct basic_header<known_magic::header<std::uint64_t> >;

static_assert( std::is_trivially_copyable<header>::value
             , "Oops"
             );
} /* ! namespace nar */

/**
 * @file    lib/include/nar/standard.hpp
 * @authors Nicolas DI PRIMA <nicolas@di-prima.fr>
 * @date    2016-04-14
 *
 * @copyright
 *    Copyright (c) 2016, Nicolas DI PRIMA. All rights reserved.
 *    This software may be modified and distributed under the terms
 *    of the BSD license. See the LICENSE file for details.
 */
#ifndef LIB_INCLUDE_NAR_STANDARD_HPP_
# define LIB_INCLUDE_NAR_STANDARD_HPP_

# include <cstdint>

# if defined(__BIG_ENDIAN__)
#  warning \
    This library does not support implementation for big endian. \
    Report this issue to: https://github.com/nar-org/libnar/issues.
# elif defined(__LITTLE_ENDIAN__)
// OK. No byte swap needed.
# else
#  error \
    We cannot detect the endianness. \
    Report this issue to: https://github.com/nar-org/libnar/issues.
# endif

namespace nar {

namespace known_magic {

template<typename T> constexpr T initiate = T(0x5d204852414e205bULL);
template<> constexpr char const* initiate<char const*> = "[ NARH ]";

template<typename T> constexpr T file = T(0x5d20454c4946205bULL);
template<> constexpr char const* file<char const*> = "[ FILE ]";

} /* ! namespace known_magic */

/**
 * @brief current library version of NAR
 * Define the current library version of NAR we have implemented.
 */
constexpr std::uint16_t current_format_version = 1;

/**
 * @brief current supported version of NAR
 * This library is compabitle with NAR down to this version
 */
constexpr std::uint16_t compatible_format_version = 1;

constexpr std::uint64_t flags_compression_1  = 1ULL << 0;
constexpr std::uint64_t flags_compression_2  = 1ULL << 1;
constexpr std::uint64_t flags_format_version = 0xFFFFFFFFULL << 46;
constexpr std::uint64_t flags_file_executable = 1ULL << 63;

} /* ! namespace nar */

#endif /** ! LIB_INCLUDE_NAR_STANDARD_HPP_  */

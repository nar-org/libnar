/**
 * @file    lib/include/nar/standard.hpp
 * @authors Nicolas DI PRIMA <nicolas@di-prima.fr>
 * @date    2016-04-14
 *
 * @Copyright Nicolas DI PRIMA
 */
#ifndef LIB_INCLUDE_NAR_STANDARD_HPP_
# define LIB_INCLUDE_NAR_STANDARD_HPP_

# include <cstdint>

namespace nar {

namespace known_magic {

template<typename T>
constexpr T header = T(0x5d204852414e205bULL);

// cppcheck-suppress syntaxError
template<>
constexpr char const* header<char const*> = "[ NARH ]";

template<typename T>
constexpr T file = T(0x5d20454c4946205bULL);

// cppcheck-suppress unmatchedSuppression syntaxError
template<>
constexpr char const* file<char const*> = "[ FILE ]";

} /* ! namespace known_magic */

/**
 * @brief current library version of NAR
 *
 * Define the current library version of NAR we have implemented.
 *
 * @{
 */
constexpr std::uint64_t current_version_major = 0;
constexpr std::uint64_t current_version_minor = 1;
constexpr std::uint64_t current_version = (current_version_major << 32)
                                        | current_version_minor;
/** @} */

/**
 * @brief current supported version of NAR
 *
 * This library is compabitle with NAR down to this version
 *
 * @{
 */
constexpr std::uint64_t compatible_version_major = 0;
constexpr std::uint64_t compatible_version_minor = 1;
constexpr std::uint64_t compatible_version = (compatible_version_major << 32)
                                           | compatible_version_minor;
/** @} */


} /* ! namespace nar */

#endif /** ! LIB_INCLUDE_NAR_STANDARD_HPP_  */

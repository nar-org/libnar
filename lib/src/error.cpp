/**
 * @file    lib/src/error.cpp
 * @authors Nicolas DI PRIMA <nicolas@di-prima.fr>
 * @date    2016-05-01
 *
 * @Copyright Nicolas DI PRIMA
 */

#include "nar/error.hpp"

namespace nar {
namespace error {

invalid_magic_number::invalid_magic_number( std::uint64_t expected
                                          , std::uint64_t received
                                          )
  : expected_(expected)
  , received_(received) {
}
char const* invalid_magic_number::what() const noexcept {
  return "NAR: invalid magic number";
}

invalid_header_length::invalid_header_length( std::uint64_t expected
                                            , std::uint64_t received
                                            )
  : expected_(expected)
  , received_(received) {
}
char const* invalid_header_length::what() const noexcept {
  return "NAR: invalid header length";
}

} /* ! namespace error */
} /* ! namespace nar */

/**
 * @file    lib/include/nar/error.hpp
 * @authors Nicolas DI PRIMA <nicolas@di-prima.fr>
 * @date    2016-04-09
 *
 * @Copyright Nicolas DI PRIMA
 */
#ifndef LIB_INCLUDE_NAR_ERROR_HPP_
# define LIB_INCLUDE_NAR_ERROR_HPP_

# include <cstdint>

# include <exception>

namespace nar {

struct exception : virtual ::std::exception { };

namespace error {

struct invalid_magic_number : virtual nar::exception {
  invalid_magic_number( std::uint64_t expected
                      , std::uint64_t received
                      );
  virtual char const* what() const noexcept;
  std::uint64_t const expected_;
  std::uint64_t const received_;
};

struct invalid_header_length : virtual nar::exception {
  invalid_header_length( std::uint64_t expected
                       , std::uint64_t received
                       );
  virtual char const* what() const noexcept;
  std::uint64_t const expected_;
  std::uint64_t const received_;
};

} /* ! namespace error */
} /* ! namespace nar */

# define NAR_ASSERT_MAGIC(expected, received)                     \
  do {                                                            \
    if (expected != received) {                                   \
      throw nar::error::invalid_magic_number(expected, received); \
    }                                                             \
  } while(false)

#endif /* ! LIB_INCLUDE_NAR_ERROR_HPP_ */

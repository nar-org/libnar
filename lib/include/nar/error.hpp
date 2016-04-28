/**
 * @file    lib/include/nar/error.hpp
 * @authors Nicolas DI PRIMA <nicolas@di-prima.fr>
 * @date    2016-04-09
 *
 * @Copyright Nicolas DI PRIMA
 */
#ifndef LIB_INCLUDE_NAR_ERROR_HPP_
# define LIB_INCLUDE_NAR_ERROR_HPP_

# include <boost/exception/all.hpp>

namespace nar {
struct exception
    : virtual ::boost::exception
    , virtual ::std::exception
{
};

namespace error {
typedef boost::error_info< struct tag_magic_read
                         , std::uint64_t
                         > magic_read;
typedef boost::error_info< struct tag_magic_expected
                         , std::uint64_t
                         > magic_expected;
typedef boost::error_info< struct tag_length_read
                         , std::uint64_t
                         > length_read;
typedef boost::error_info< struct tag_length_write
                         , std::uint64_t
                         > length_write;
typedef boost::error_info< struct tag_length_expected
                         , std::uint64_t
                         > length_expected;

} /* ! namespace error */

template <class ErrorInfo,class E>
typename ErrorInfo::error_info::value_type const* get_error_info(E const& x) {
  return ::boost::get_error_info(x);
}
template <class ErrorInfo,class E>
typename ErrorInfo::error_info::value_type*       get_error_info(E& x) {
  return ::boost::get_error_info(x);
}
} /* ! namespace nar */

# if defined(NDEBUG)
#  define NAR_ERROR_DETAILS()
# else
#  define NAR_ERROR_DETAILS()                                              \
        << ::boost::throw_function(BOOST_THROW_EXCEPTION_CURRENT_FUNCTION) \
        << ::boost::throw_file(__FILE__)                                   \
        << ::boost::throw_line(__LINE__)
# endif

# define NAR_ASSERT_MAGIC(expected, received)    \
  do {                                           \
    if (expected != received) {                  \
      throw nar::exception() NAR_ERROR_DETAILS() \
        << error::magic_read(h.magic)            \
        << error::magic_expected(MAGIC);         \
    }                                            \
  } while(false)


#endif /* ! LIB_INCLUDE_NAR_ERROR_HPP_ */

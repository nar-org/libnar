/**
 * @file    lib/include/nar/serialisation.hpp
 * @authors Nicolas DI PRIMA <nicolas@di-prima.fr>
 * @date    2016-04-09
 *
 * @copyright
 *    Copyright (C) 2016, Nicolas DI PRIMA. All rights reserved.
 *
 *    This software may be modified and distributed under the terms
 *    of the BSD license. See the LICENSE file for details.
 *
 * @brief stream operators
 */
#ifndef LIB_INCLUDE_NAR_SERIALISATION_HPP_
# define LIB_INCLUDE_NAR_SERIALISATION_HPP_

# include <array>
# include <istream>
# include <ostream>

# include <nar/error.hpp>
# include <nar/generic/header.hpp>
# include <nar/generic/item.hpp>
# include <nar/header.hpp>
# include <nar/item.hpp>

namespace nar {
namespace generic {

template<class T>
typename std::enable_if< is_basic_header<T>::value || is_basic_item<T>::value
                       , T
                       >::type
read(std::istream& is) {
  T t;

  is.read(reinterpret_cast<char*>(&t), sizeof(t));
  if (is.gcount() != sizeof(t)) {
    throw nar::exception() NAR_ERROR_DETAILS()
      << error::length_read(is.gcount())
      << error::length_expected(sizeof(t));
  }

  return std::move(t);
}

} /* ! namespace generic */

template<class T>
typename std::enable_if< is_basic_header<T>::value
                       , T
                       >::type
read(std::istream& is) {
  return T(generic::read<generic::header>(is));
};
template<class T>
typename std::enable_if< is_basic_item<T>::value
                       , T
                       >::type
read(std::istream& is) {
  return T(generic::read<generic::item>(is));
};

template<class T>
typename std::enable_if< is_basic_header<T>::value || is_basic_item<T>::value
                       , std::ostream&
                       >::type
write(std::ostream& os, T const& t) {
  os.write(reinterpret_cast<char const*>(&t), sizeof(t));
  return os;
}

} /* ! namespace nar */

template<class T>
typename std::enable_if< nar::is_basic_header<T>::value ||
                         nar::is_basic_item<T>::value
                       , std::istream&
                       >::type
operator>> (std::istream& is, T& t) {
  t = nar::read<T>(is);
  return is;
};
template<class T>
typename std::enable_if< nar::is_basic_header<T>::value ||
                         nar::is_basic_item<T>::value
                       , std::ostream&
                       >::type
operator<< (std::ostream& os, T const& t) {
  return nar::write<T>(os, t);
};

#endif /** ! LIB_INCLUDE_NAR_SERIALISATION_HPP_  */

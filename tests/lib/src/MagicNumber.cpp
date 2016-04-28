/**
 * @file    tests/lib/src/MagicNumber.cpp
 * @authors Nicolas DI PRIMA <nicolas@di-prima.fr>
 * @date    2016-04-09
 *
 * @Copyright Nicolas DI PRIMA
 */

#include <sstream>
#include <string>

#include "nar/standard.hpp"

#include "gtest/gtest.h"

using nar;

TEST(MagicNumberTest, NarHeader)
{
    std::string const magic_str(known_magic::header<char const*>);

    ASSERT_EQ(magic_str.size(), sizeof(known_magic::header<std::uint64_t>));

    std::uint64_t const magic =
       *(reinterpret_cast<std::uint64_t const*>(magic_str.data()));

    ASSERT_EQ(magic, nar::known_magic::header<std::uint64_t>);
}

TEST(MagicNumberTest, NarFile)
{
    std::string const magic_str(nar::known_magic::file<char const*>);

    ASSERT_EQ(magic_str.size(), sizeof(nar::known_magic::file<std::uint64_t>));

    std::uint64_t const magic =
      *(reinterpret_cast<std::uint64_t const*>(magic_str.data()));

    ASSERT_EQ(magic, nar::known_magic::file<std::uint64_t>);
}

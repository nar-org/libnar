/**
 * @file    tests/lib/src/SerialisationHeader.cpp
 * @authors Nicolas DI PRIMA <nicolas@di-prima.fr>
 * @date    2016-04-18
 *
 * @Copyright Nicolas DI PRIMA
 */

#include <sstream>
#include <string>
#include <vector>

#include "gtest/gtest.h"

#include "nar/standard.hpp"
#include "nar/generic/header.hpp"
#include "nar/generic/item.hpp"
#include "nar/serialisation.hpp"

TEST(NarUnitTests, NarHeaderParseOK)
{
    std::stringstream ss("[ NARH ]"
                         "........"
                         "........"
                         "........"
                         "........"
                         "........"
                         "........"
                         "........"
                         "........"
                         "........"
                         "\0\0\0\0\0\0\0\0"
                         "\0\0\0\0\0\0\0\0"
                        );

    nar::header header;

    ASSERT_NO_THROW({header = nar::read<nar::header>(ss);});

    ASSERT_TRUE(ss.good());

    ASSERT_EQ(header.magic, nar::known_magic::header<std::uint64_t>);
}
TEST(NarUnitTests, NarHeaderParseNotEnoughBytes)
{
  using nar::error;
  using boost;
  std::stringstream ss("[ NARH ]"
                       "........"
                       "........"
                       "........"
                       "........"
                       "........"
                       "........"
                      );
  const std::size_t ss_size = 7 * 8;
  nar::header header;

  ASSERT_NO_THROW({
    try {
      header = nar::read<nar::header>(ss);
    } catch (nar::exception const& e) {
      std::uint64_t const* read     = get_error_info<error::length_read>(e);
      std::uint64_t const* expected = get_error_info<error::length_expected>(e);

      ASSERT_NE(read, nullptr);
      ASSERT_NE(expected, nullptr);
      ASSERT_EQ(*read    , ss_size);
      ASSERT_EQ(*expected, sizeof(header));
    }
  });
}
TEST(NarUnitTests, NarHeaderParseWrongMagic)
{
  using nar::error;
  using boost;
  std::stringstream ss("<@@@@@@@>"
                       "........"
                       "........"
                       "........"
                       "........"
                       "........"
                       "........"
                       "........"
                      );
  nar::header header;

  ASSERT_NO_THROW({
    try {
      header = nar::read<nar::header>(ss);
    } catch (nar::exception const& e) {
      std::uint64_t const* read     = get_error_info<error::magic_read>(e);
      std::uint64_t const* expected = get_error_info<error::magic_expected>(e);
      std::uint64_t const magic = nar::known_magic::header<std::uint64_t>;

      std::uint64_t t;
      memcpy(reinterpret_cast<char*>(&t), "<@@@@@@@@>", 8);

      ASSERT_NE(read, nullptr);
      ASSERT_NE(expected, nullptr);
      ASSERT_EQ(*read    , t);
      ASSERT_EQ(*expected, magic);
    }
  });
}
TEST(NarUnitTests, NarHeaderParseWrongMagicNotEnoughBytes)
{
    std::stringstream ss("<@@@@@@@>"
                         "........"
                         "........"
                         "........"
                        );
    nar::header header;
    ASSERT_THROW(header = nar::read<nar::header>(ss), nar::exception);
}

namespace {

struct Unit
{
  std::uint64_t const version;
  std::uint64_t const cipher;
  std::uint64_t const compression;
  std::uint64_t const signature_position;
  std::uint64_t const index_position;
  std::uint64_t const unused_1;
  std::uint64_t const unused_2;
};

class NarSerialiseHeader
  : public ::testing::TestWithParam<Unit>
{
};

TEST_P(NarSerialiseHeader, Property)
{
  Unit const unit = GetParam();

  nar::header header;
  header.version = unit.version;
  header.cipher = unit.cipher;
  header.compression = unit.compression;
  header.signature_position = unit.signature_position;
  header.index_position = unit.index_position;
  header.unused_1 = unit.unused_1;
  header.unused_2 = unit.unused_2;

  std::stringstream ss;

  nar::header header_1;
  ss << header;
  ss >> header_1;

  ASSERT_EQ(header, header_1);
}

static std::vector<Unit> const tests =
{ { 0, 0, 0, 0, 0, 0, 0 }
, { 1, 0, 0, 0, 0, 0, 0 }
, { 1, 1, 0, 0, 0, 0, 0 }
, { 1, 1, 1, 0, 0, 0, 0 }
, { 1, 1, 0, 1, 0, 0, 0 }
, { 1, 1, 0, 0, 1, 0, 0 }
, { 1, 1, 0, 0, 0, 1, 0 }
, { 1, 1, 0, 0, 0, 0, 1 }
, { 1, 1, 1, 1, 1, 1, 1 }
};

INSTANTIATE_TEST_CASE_P( PropertyTests
                       , NarSerialiseHeader
                       , ::testing::ValuesIn(tests)
                       );
} /* ! namespace */

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
#include "nar/header.hpp"
#include "nar/serialisation.hpp"

TEST(NarUnitTests, NarHeaderParseOK)
{
    std::stringstream ss("[ NARH ]"
                         "........"
                         "........"
                         "........"
                        );

    nar::header::narh header;

  ASSERT_NO_THROW(ss >> header);

    ASSERT_TRUE(ss.good());

    ASSERT_EQ(header.magic, nar::known_magic::narh<std::uint64_t>);
  ASSERT_EQ(header.magic, nar::header::narh::value);
}
TEST(NarUnitTests, NarHeaderParseNotEnoughBytes)
{
  using namespace nar;
  std::stringstream ss("[ NARH ]"
                       "........"
                       "........"
                      );
  const std::size_t ss_size = 3 * 8;
  nar::header::narh header;

  ASSERT_NO_THROW({
    try {
      ss >> header;
    } catch (nar::error::invalid_header_length const& e) {
      ASSERT_EQ(e.received_, ss_size);
      ASSERT_EQ(e.expected_, sizeof(header));
    }
  });
}
TEST(NarUnitTests, NarHeaderParseWrongMagic)
{
  using namespace nar;
  std::stringstream ss("<@@@@@@@>"
                       "........"
                       "........"
                       "........"
                      );
  nar::header::narh header;

  ASSERT_NO_THROW({
    try {
      ss >> header;
    } catch (nar::error::invalid_magic_number const& e) {
      std::uint64_t const magic = nar::known_magic::narh<std::uint64_t>;
      std::uint64_t t;
      memcpy(reinterpret_cast<char*>(&t), "<@@@@@@@@>", 8);

      ASSERT_EQ(e.received_, t);
      ASSERT_EQ(e.expected_, magic);
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
    nar::header::narh header;
    ASSERT_THROW( ss >> header , nar::exception);
}

namespace {

struct Unit
{
  std::uint64_t const flags;
  std::uint64_t const length_1;
  std::uint64_t const length_2;
};

class NarSerialiseHeader
  : public ::testing::TestWithParam<Unit>
{
};

TEST_P(NarSerialiseHeader, Property)
{
  Unit const unit = GetParam();

  nar::header::narh header;
  header.flags = unit.flags;
  header.length_1 = unit.length_1;
  header.length_2 = unit.length_2;

  std::stringstream ss;

  nar::header::narh header_1;
  ss << header;
  ss >> header_1;

  ASSERT_EQ(header, header_1);
}

static std::vector<Unit> const tests =
{ { 0, 0, 0 }
, { 0, 0, 1 }
, { 0, 1, 0 }
, { 0, 1, 1 }
, { 1, 0, 0 }
, { 1, 0, 1 }
, { 1, 1, 0 }
, { 1, 1, 1 }
};

INSTANTIATE_TEST_CASE_P( PropertyTests
                       , NarSerialiseHeader
                       , ::testing::ValuesIn(tests)
                       );
} /* ! namespace */

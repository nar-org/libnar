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
#include "nar/serialisation.hpp"

TEST(NarUnitTests, NarHeaderParseOK)
{
    std::stringstream ss("[ NARH ]"
                         "........"
                         "........"
                         "........"
                        );

    nar::header::narh header;

  ASSERT_NO_THROW({
    try {
      ss >> header;
    } catch (nar::exception const& e) {
      std::cerr << ::boost::diagnostic_information(e) << std::endl;
    } catch (std::exception const& e) {
      std::cerr << e.what() << std::endl;
    }
  });

    ASSERT_TRUE(ss.good());

    ASSERT_EQ(header.magic, nar::known_magic::narh<std::uint64_t>);
}
TEST(NarUnitTests, NarHeaderParseNotEnoughBytes)
{
  using namespace nar;
  using namespace boost;
  std::stringstream ss("[ NARH ]"
                       "........"
                       "........"
                      );
  const std::size_t ss_size = 3 * 8;
  nar::header::narh header;

  ASSERT_NO_THROW({
    try {
      ss >> header;
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
  using namespace nar;
  using namespace boost;
  std::stringstream ss("<@@@@@@@>"
                       "........"
                       "........"
                       "........"
                      );
  nar::header::narh header;

  ASSERT_NO_THROW({
    try {
      ss >> header;
    } catch (nar::exception const& e) {
      std::uint64_t const* read     = get_error_info<error::magic_read>(e);
      std::uint64_t const* expected = get_error_info<error::magic_expected>(e);
      std::uint64_t const magic = nar::known_magic::narh<std::uint64_t>;

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

/**
 * @file    tests/lib/src/SerialisationItem.cpp
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
#include "nar/item.hpp"
#include "nar/serialisation.hpp"

TEST(NarUnitTests, NarGenericParseOK)
{
    std::stringstream ss("[ @@@@ ]"
                         "........"
                         "........"
                         "........"
                        );

    nar::generic::header generic;

    ASSERT_NO_THROW({generic = nar::read<nar::generic::header>(ss);});

    ASSERT_TRUE(ss.good());
}

TEST(NarUnitTests, NarFileParseOK)
{
    std::stringstream ss("[ FILE ]"
                         "........"
                         "........"
                         "........"
                        );

    nar::file file;

    ASSERT_NO_THROW({file = nar::read<nar::file>(ss);});

    ASSERT_TRUE(ss.good());

    ASSERT_EQ(file.magic, nar::known_magic::file<std::uint64_t>);
}
TEST(NarUnitTests, NarFileParseWrongMagic)
{
    std::stringstream ss("<@@@@@@@>"
                         "........"
                         "........"
                         "........"
                        );
    nar::file file;
    ASSERT_THROW(file = nar::read<nar::file>(ss), nar::exception);
}

namespace {
struct Unit
{
  std::uint64_t const flags;
  std::uint64_t const length_1;
  std::uint64_t const length_2;
};

class NarSerialiseItem
  : public ::testing::TestWithParam<Unit>
{
};

TEST_P(NarSerialiseItem, Property)
{
  Unit const unit = GetParam();

  nar::file file;
  file.flags = unit.flags;
  file.length_1 = unit.length_1;
  file.length_2 = unit.length_2;

  std::stringstream ss;

  nar::file file_1;
  ss << file;
  ss >> file_1;

  ASSERT_EQ(file, file_1);
}

static std::vector<Unit> const tests =
{ { 0, 0, 0 }
, { 0xdeaddeaddeaddead, 0x42, 0xdeadbeefdeadbeef }
};

INSTANTIATE_TEST_CASE_P( PropertyTests
                       , NarSerialiseItem
                       , ::testing::ValuesIn(tests)
                       );
} /* ! namespace */

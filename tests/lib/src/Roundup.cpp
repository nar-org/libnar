/**
 * @file    tests/lib/src/Roundup.cpp
 * @authors Nicolas DI PRIMA <nicolas@di-prima.fr>
 * @date    2016-04-18
 *
 * @Copyright Nicolas DI PRIMA
 */

#include <sstream>
#include <string>
#include <vector>

#include "nar/roundup.hpp"

#include "gtest/gtest.h"

struct Unit
{
  std::uint64_t const input;
  std::uint64_t const expected_roundup;
  std::uint64_t const expected_padding;
};

class NarRoundup
  : public ::testing::TestWithParam<Unit>
{
};

TEST_P(NarRoundup, Roundup64Test)
{
  Unit const unit = GetParam();
  std::uint64_t const roundup = nar::ROUND_UP64(unit.input);
  ASSERT_EQ(unit.expected_roundup, roundup);
}
TEST_P(NarRoundup, Padding64Test)
{
  Unit const unit = GetParam();
  std::uint64_t const padding = nar::PADDING64(unit.input);
  ASSERT_EQ(unit.expected_padding, padding);
}

std::vector<Unit> const tests =
{ { 0,  0, 0 }
, { 1,  8, 7 }
, { 2,  8, 6 }
, { 3,  8, 5 }
, { 4,  8, 4 }
, { 5,  8, 3 }
, { 6,  8, 2 }
, { 7,  8, 1 }
, { 8,  8, 0 }
, { 9, 16, 7 }
};

INSTANTIATE_TEST_CASE_P( Roundup64UnitTests
                       , NarRoundup
                       , ::testing::ValuesIn(tests)
                       );

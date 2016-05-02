/**
 * @file    tests/lib/src/SerialisationHeader.cpp
 * @authors Nicolas DI PRIMA <nicolas@di-prima.fr>
 * @date    2016-05-02
 *
 * @Copyright Nicolas DI PRIMA
 *    Copyright (c) 2016, Nicolas DI PRIMA. All rights reserved.
 *    This software may be modified and distributed under the terms
 *    of the BSD license. See the LICENSE file for details.
 */

#include <sstream>
#include <string>
#include <vector>

#include "gtest/gtest.h"

#include "nar/standard.hpp"
#include "nar/header.hpp"
#include "nar/serialisation.hpp"

namespace {

class NarHeaderInitiateVersion
  : public ::testing::TestWithParam<std::uint16_t>
{
};

TEST_P(NarHeaderInitiateVersion, SetAndGet)
{
  std::uint16_t const unit = GetParam();
  nar::header::initiate header;
  nar::version(header, unit);
  ASSERT_EQ(unit, nar::version(header));
}

TEST_P(NarHeaderInitiateVersion, Serialisation)
{
  std::stringstream ss( std::ios_base::in
                      | std::ios_base::out
                      | std::ios_base::binary);
  std::uint16_t const unit = GetParam();
  nar::header::initiate header_1;
  nar::header::initiate header_2;
  nar::version(header_1, unit);

  ss << header_1;
  ss >> header_2;
  ASSERT_EQ(nar::version(header_1), nar::version(header_2));
  ASSERT_EQ(header_1, header_2);
}

static std::vector<std::uint16_t> const tests =
{ 0
, 1
, 0xFF
, 0x1010
, 0xFFFF
};

INSTANTIATE_TEST_CASE_P( PropertyTests
                       , NarHeaderInitiateVersion
                       , ::testing::ValuesIn(tests)
                       );
} /* ! namespace */

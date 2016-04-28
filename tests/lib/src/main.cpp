/**
 * @file    tests/lib/src/main.cpp
 * @authors Nicolas DI PRIMA <nicolas@di-prima.fr>
 * @date    2016-03-20
 *
 * @Copyright Nicolas DI PRIMA
 */

#include "gtest/gtest.h"

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

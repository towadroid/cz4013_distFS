//
// Created by towa-ubuntu on 3/19/20.
//

#include "gtest/gtest.h"
#include "../../utils/utils.hpp"

void test_cache_bounds(int offset, int count, int chunck_size, int exp_lower, int exp_upper) {
    int lower, upper;
    utils::internals2::calculate_bounds(lower, upper, offset, count, chunck_size);
    EXPECT_EQ(exp_lower, lower);
    EXPECT_EQ(exp_upper, upper);
}

TEST(Cache_bound, aligned) {
    test_cache_bounds(0, 100, 100, 0, 100);
}

TEST(Cache_bound, lower_not_aligned) {
    test_cache_bounds(5, 83, 100, 0, 100);
}

TEST(Cache_bound, one) {
    test_cache_bounds(0, 101, 100, 0, 200);
}

TEST(Cache_bound, two) {
    test_cache_bounds(3, 100, 100, 0, 200);
}

TEST(Cache_bound, three) {
    test_cache_bounds(3, 103, 100, 0, 200);
}

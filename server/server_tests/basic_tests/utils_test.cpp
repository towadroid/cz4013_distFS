//
// Created by towa-ubuntu on 3/13/20.
//

#include "gtest/gtest.h"
#include "../../utils.hpp"
#include <string>
#include "../../constants.hpp"

unsigned char buffer[1024];

TEST(Packing, pack_integers) {
    utils::packi32(buffer, 0x03002684);
    EXPECT_EQ(buffer[0], 0x03);
    EXPECT_EQ(buffer[1], 0x00);
    EXPECT_EQ(buffer[2], 0x26);
    EXPECT_EQ(buffer[3], 0x84);

    utils::packi32(buffer, 0xF3002684);
    EXPECT_EQ(buffer[0], 0xF3);
    EXPECT_EQ(buffer[1], 0x00);
    EXPECT_EQ(buffer[2], 0x26);
    EXPECT_EQ(buffer[3], 0x84);

    utils::packi16(buffer, 0xF3002684);
    EXPECT_EQ(buffer[0], 0x26);
    EXPECT_EQ(buffer[1], 0x84);
}

// 0xF3002684 = -218093948
TEST(Packing, unpack_integers) {
    buffer[0] = 0xF3;
    buffer[1] = 0x00;
    buffer[2] = 0x26;
    buffer[3] = 0x84;
    int value_signed = 0xF3002684;
    unsigned int value_unsigned = 0xF3002684;
    EXPECT_EQ(utils::unpacki32(buffer), value_signed);
    EXPECT_EQ(utils::unpacku32(buffer), value_unsigned);
}

// "a bc" = 61 20 62 63
TEST(Packing, pack_string) {
    std::string a{"a bc"};
    utils::pack_str(buffer, a);
    unsigned char *cur = buffer;
    EXPECT_EQ(*cur++, 0x0);
    EXPECT_EQ(*cur++, 0x0);
    EXPECT_EQ(*cur++, 0x0);
    EXPECT_EQ(*cur++, 0x4);
    EXPECT_EQ(*cur++, 0x61);
    EXPECT_EQ(*cur++, 0x20);
    EXPECT_EQ(*cur++, 0x62);
    EXPECT_EQ(*cur++, 0x63);

    std::string res = utils::unpack_str(buffer);
    EXPECT_TRUE(0 == a.compare(res));
}

TEST(Time, calculate_future_time) {
    auto start = std::chrono::steady_clock::now() + std::chrono::seconds(10);
    int s, usec;
    utils::future_duration_to_s_usec(start, s, usec);
    EXPECT_EQ(s, 9);
    EXPECT_GE(usec, 999000);
}

bool compare(const unsigned char *a, const unsigned char *b, int n) {
    bool result = true;
    for (int i = 0; i < n; ++i) {
        result &= (a[i] == b[i]);
        EXPECT_EQ(a[i], b[i]);
    }
    return result;
}


TEST(Packing, pack_variadic) {
    std::string a{"abc"};
    unsigned char bla[16];
    for (unsigned char i = 0; i < 16; ++i) bla[i] = i;
    unsigned int n = utils::calculate_pack_size("ciisbi", a.c_str(), 16);

    EXPECT_EQ(36, n);

    auto result = new unsigned char[n];
    unsigned char tmp[32];
    utils::pack(result, "ciisbi", 'a', 1, 2, a.c_str(), 16, bla, 3);

    int cur = 0;
    EXPECT_EQ(result[0], 'a');
    cur += 1;

    utils::packi32(tmp, 1);
    EXPECT_TRUE(compare(result + cur, tmp, 4));
    cur += 4;

    utils::packi32(tmp, 2);
    EXPECT_TRUE(compare(result + cur, tmp, 4));
    cur += 4;

    utils::pack_str(tmp, a);
    EXPECT_TRUE(compare(result + cur, tmp, 7));
    cur += 7;

    EXPECT_TRUE(compare(result + cur, bla, 16));
    cur += 16;

    utils::packi32(tmp, 3);
    EXPECT_TRUE(compare(result + cur, tmp, 4));
    cur += 4;

    delete[] result;
}

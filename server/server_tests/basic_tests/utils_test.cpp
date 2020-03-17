//
// Created by towa-ubuntu on 3/13/20.
//

#include "gtest/gtest.h"
#include "../../utils/utils.hpp"
#include <string>
#include "../../constants.hpp"
#include "../../utils/packing.hpp"

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

TEST(Packing, pack_variadic_template) {
    std::string b{"abc"}, b2;
    unsigned char bla[16];
    for (unsigned char i = 0; i < 16; ++i) bla[i] = i;

    char a = 'a', a2;
    int one = 1, one2;
    int two = 2, two2;
    int three = 3, three2;

    unsigned char tmp[32];
    BytePtr result_ptr;
    utils::pack(result_ptr, a, one, two, b, (unsigned int) 16, bla, three);

    unsigned char *result = result_ptr.get();

    int cur = 0;
    EXPECT_EQ(result[0], a);
    cur += 1;

    utils::packi32(tmp, one);
    EXPECT_TRUE(compare(result + cur, tmp, 4));
    cur += 4;

    utils::packi32(tmp, two);
    EXPECT_TRUE(compare(result + cur, tmp, 4));
    cur += 4;

    utils::pack_str(tmp, b);
    EXPECT_TRUE(compare(result + cur, tmp, 7));
    cur += 7;

    EXPECT_TRUE(compare(result + cur, bla, 16));
    cur += 16;

    utils::packi32(tmp, three);
    EXPECT_TRUE(compare(result + cur, tmp, 4));
    cur += 4;

    //use 4 int for the byte[16]
    unsigned int k, l, m, n;

    utils::unpack(result, a2, one2, two2, b2, k, l, m, n, three2);
    EXPECT_EQ(a, a2);
    EXPECT_EQ(one, one2);
    EXPECT_EQ(two, two2);
    EXPECT_EQ(k, utils::unpacku32(bla));
    EXPECT_EQ(l, utils::unpacku32(bla + 4));
    EXPECT_EQ(m, utils::unpacku32(bla + 8));
    EXPECT_EQ(n, utils::unpacku32(bla + 12));
    EXPECT_EQ(b, b2);
    EXPECT_EQ(three, three2);
}
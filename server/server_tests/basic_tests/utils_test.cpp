//
// Created by towa-ubuntu on 3/13/20.
//

#include "gtest/gtest.h"
#include "../../utils/utils.hpp"
#include <string>
#include "../../constants.hpp"
#include "../../utils/packing.hpp"

TEST(Packing, pack_integers) {
    BytePtr buffer;
    unsigned int tmp = 0x03002684;
    utils::pack(buffer, tmp);
    EXPECT_EQ(buffer[0], 0x03);
    EXPECT_EQ(buffer[1], 0x00);
    EXPECT_EQ(buffer[2], 0x26);
    EXPECT_EQ(buffer[3], 0x84);

    tmp = 0xF3002684;
    utils::pack(buffer, tmp);
    EXPECT_EQ(buffer[0], 0xF3);
    EXPECT_EQ(buffer[1], 0x00);
    EXPECT_EQ(buffer[2], 0x26);
    EXPECT_EQ(buffer[3], 0x84);
}

// 0xF3002684 = -218093948
TEST(Packing, unpack_integers) {
    BytePtr buffer = BytePtr(new unsigned char[4]);
    buffer[0] = 0xF3;
    buffer[1] = 0x00;
    buffer[2] = 0x26;
    buffer[3] = 0x84;
    int value_signed = 0xF3002684, value_s_unpacked;
    unsigned int value_unsigned = 0xF3002684;
    unsigned int value_u_unpacked;
    utils::unpack(buffer.get(), value_s_unpacked);
    utils::unpack(buffer.get(), value_u_unpacked);
    EXPECT_EQ(value_s_unpacked, value_signed);
    EXPECT_EQ(value_u_unpacked, value_unsigned);
}

// "a bc" = 61 20 62 63
TEST(Packing, pack_string) {
    BytePtr buffer;
    std::string a{"a bc"};
    utils::pack(buffer, a);
    unsigned char *cur = buffer.get();
    EXPECT_EQ(*cur++, 0x0);
    EXPECT_EQ(*cur++, 0x0);
    EXPECT_EQ(*cur++, 0x0);
    EXPECT_EQ(*cur++, 0x4);
    EXPECT_EQ(*cur++, 0x61);
    EXPECT_EQ(*cur++, 0x20);
    EXPECT_EQ(*cur++, 0x62);
    EXPECT_EQ(*cur++, 0x63);

    std::string res;
    utils::unpack(buffer.get(), res);
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

TEST(Packing, pack_variadic_template) {
    std::string b{"abc"}, b2;
    unsigned char bla[16];
    for (unsigned char i = 0; i < 16; ++i) bla[i] = i;

    char a = 'a', a2;
    int one = 1, one2;
    int two = 2, two2;
    int three = 3, three2;

    BytePtr tmp;
    BytePtr result_ptr;
    utils::pack(result_ptr, a, one, two, b, (unsigned int) 16, bla, three);

    unsigned char *result = result_ptr.get();

    int cur = 0;
    EXPECT_EQ(result[0], a);
    cur += 1;

    utils::pack(tmp, one);
    EXPECT_TRUE(compare(result + cur, tmp.get(), 4));
    cur += 4;

    utils::pack(tmp, two);
    EXPECT_TRUE(compare(result + cur, tmp.get(), 4));
    cur += 4;

    utils::pack(tmp, b);
    EXPECT_TRUE(compare(result + cur, tmp.get(), 7));
    cur += 7;

    EXPECT_TRUE(compare(result + cur, bla, 16));
    cur += 16;

    utils::pack(tmp, three);
    EXPECT_TRUE(compare(result + cur, tmp.get(), 4));
    cur += 4;

    //use 4 int for the byte[16]
    unsigned int k, l, m, n, k2, l2, m2, n2;

    utils::unpack(result, a2, one2, two2, b2, k, l, m, n, three2);
    EXPECT_EQ(a, a2);
    EXPECT_EQ(one, one2);
    EXPECT_EQ(two, two2);

    utils::unpack(bla, k2);
    utils::unpack(bla + 4, l2);
    utils::unpack(bla + 8, m2);
    utils::unpack(bla + 12, n2);

    EXPECT_EQ(k, k2);
    EXPECT_EQ(l, l2);
    EXPECT_EQ(m, m2);
    EXPECT_EQ(n, n2);
    EXPECT_EQ(b, b2);
    EXPECT_EQ(three, three2);
}

TEST(PACKING, simple_int) {
    int n = 35;
    BytePtr a;
    utils::pack(a, n);
    printf("%X %X %X %X", a[0], a[1], a[2], a[3]);
}
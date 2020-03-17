//
// Created by towa-ubuntu on 3/17/20.
//

#include <cstring>
#include "packing_low_level.hpp"

/**Packs an integer with n bits.
 *
 * Assuming n % 8 == 0. Buffer needs to be n bits long.
 *
 * @tparam T type of int
 * @param n amount of bits
 * @param[out] buffer where data is stored
 * @param x integer to pack
 */
template<class T>
void packin(int n, unsigned char *buf, const T x) {
    n -= 8;
    for (; n >= 0; n = n - 8) {
        *buf = x >> n; //"x >> n" will be truncated, only lowest byte is stored
        buf++;
    }
}


void internals::packi16(unsigned char *buf, const unsigned short int x) {
    packin<unsigned short int>(16, buf, x);
}

void internals::packi32(unsigned char *buf, const unsigned int x) {
    packin<unsigned int>(32, buf, x);
}

void internals::packi64(unsigned char *buf, const unsigned long long int x) {
    packin<unsigned long long int>(64, buf, x);
}

template<class T>
T unpackun(int n, unsigned char const *buf) {
    T result = 0;
    for (int i = 0; i < n / 8; ++i) {
        result = result | buf[i] << (n - 8 - i * 8);
    }
    return result;
}

template<class T>
T unpackin(int n, const unsigned char *buf) {
    T result = unpackun<T>(n, buf);
    T helper = (1 << (n - 1)) - 1;
    if (result > helper)
        result = -1 - (T) (helper - result);
    return result;
}

unsigned int internals::unpacku32(const unsigned char *buf) {
    return unpackun<unsigned int>(32, buf);
}

signed int internals::unpacki32(const unsigned char *buf) {
    return unpackin<signed int>(32, buf);
}

unsigned long long int internals::unpacku64(const unsigned char *buf) {
    return unpackin<unsigned long long int>(64, buf);
}

/** Packs a string into a buffer.
 *
 * Buffer should be large enough to hold the string and four additional bytes.
 *
 * @param[out] buf pointer to a buffer, first four bytes store the length of str
 * @param[in] str string to be packed
 */
void internals::pack_str(unsigned char *buf, const std::string &str) {
    short int len = str.length(); //returns a value of type size_t
    //we only use the lower 4 bytes
    packi32(buf, len);
    buf += 4;
    memcpy(buf, str.c_str(), len);
}

/** Unpacks a string from a given byte array
 *
 * @param[in] buf pointer to a buffer, first four bytes indicate length of following string
 * @return
 */
std::string internals::unpack_str(unsigned char *buf) {
    size_t n = unpacku32(buf);
    return std::string(reinterpret_cast<const char *>(buf + 4), n);
}


void internals::pack_float(unsigned char *buf, float x) {
    int tmp;
    memcpy(&tmp, &x, 4);
    packi32(buf, tmp);
}

void internals::pack_double(unsigned char *buf, double x) {
    unsigned long long int tmp;
    memcpy(&tmp, &x, 4);
    packi64(buf, tmp);
}
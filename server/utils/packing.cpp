//
// Created by towa-ubuntu on 3/17/20.
//

#include "packing.hpp"

unsigned int internals::calc_size() {
    return 0;
}

unsigned int internals::calc_size(const char) {
    return 1;
}

unsigned int internals::calc_size(const int) {
    return 4;
}

unsigned int internals::calc_size(const unsigned int) {
    return 4;
}

unsigned int internals::calc_size(const std::string &a) {
    return 4 + (unsigned int) a.length();
}

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

unsigned int internals::pack(unsigned char *) {
    return 0;
}

unsigned int internals::pack(unsigned char *result, const char a) {
    *result = (unsigned char) a;
    return calc_size(a);
}

unsigned int internals::pack(unsigned char *result, const int a) {
    packin<int>(32, result, a);
    return calc_size(a);
}

unsigned int internals::pack(unsigned char *result, const unsigned int a) {
    packin<unsigned int>(32, result, a);
    return calc_size(a);
}

void internals::pack(unsigned char *result, const float a) {
    unsigned int tmp;
    memcpy(&tmp, &a, 4);
    pack(result, tmp);
}

/** Packs a string into a buffer.
 *
 * Buffer should be large enough to hold the string and four additional bytes.
 *
 * @param[out] result pointer to a buffer, first four bytes store the length of str
 * @param[in] str string to be packed
 */
unsigned int internals::pack(unsigned char *result, const std::string &str) {
    int len = (int) str.length(); //returns a value of type size_t
    internals::pack(result, len);
    memcpy(result + 4, str.c_str(), (size_t) len);
    return calc_size(str);
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

unsigned int internals::unpack(unsigned char *result, char &a) {
    a = (char) *result;
    return calc_size(a);
}

unsigned int internals::unpack(unsigned char *result, int &a) {
    a = unpackin<int>(32, result);
    return calc_size(a);
}

unsigned int internals::unpack(unsigned char *result, unsigned int &a) {
    a = unpackun<unsigned int>(32, result);
    return calc_size(a);
}

/** Unpacks a string from a given byte array
 *
 * @param[in] buf pointer to a buffer, first four bytes indicate length of following string
 * @return
 */
unsigned int internals::unpack(unsigned char *result, std::string &a) {
    unsigned int n;
    unpack(result, n);
    a = std::string(reinterpret_cast<const char *>(result + 4), n);
    return calc_size(a);
}
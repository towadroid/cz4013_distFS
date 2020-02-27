//
// Created by towa-ubuntu on 2/27/20.
//

#include <limits>
#include <stdexcept>
#include <iostream>
#include "utils.hpp"
#include "globalvar.hpp"

// pack integer with n bits, assuming n % 8 == 0
// buffer needs to be n/8 bytes long
template<class T>
void packin(int n, unsigned char *buf, const T x) {
    n -= 8;
    for (; n >= 0; n = n - 8) {
        *buf = (x >> n); //"x >> n" will be truncated, only lowest byte is stored
        buf++;
    }
}

void utils::packi32(unsigned char *buf, const unsigned int x) {
    packin<unsigned int>(32, buf, x);
}

template<class T>
T unpackun(int n, unsigned char *buf) {
    T result = 0;
    for (int i = 0; i < n / 8; ++i) {
        result = result | buf[i] << (n - 8 - i * 8);
    }
    return result;
}

template<class T>
T unpackin(int n, unsigned char *buf) {
    T result = unpackun<T>(n, buf);
    T helper = (1 << (n - 1)) - 1;
    if (result > helper)
        result = -1 - (T) (helper - result);
    return result;
}

unsigned int utils::unpacku32(unsigned char *buf) {
    return unpackun<unsigned int>(32, buf);
}

signed int utils::unpacki32(unsigned char *buf) {
    return unpackin<signed int>(32, buf);
}

/*
 *  int a=-20;
    printf("%X  %X  %X %X %d\n", a>>24, a>>16, a>>8, a, a);
    unsigned char buf[4];
    utils::packi32(buf, a);
    for(int i=0; i<4; i++)
        printf("%02X", buf[i]);
    int x=utils::unpacki32(buf);
    unsigned int y = utils::unpacku32(buf);
    cout << endl << x << " " << y;
    printf("  %08X  %08X", x,y);
 *
 */
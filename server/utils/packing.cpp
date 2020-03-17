//
// Created by towa-ubuntu on 3/17/20.
//

#include "packing.hpp"

unsigned int internals::calc_size(char) {
    return 1;
}

unsigned int internals::calc_size(int) {
    return 4;
}

unsigned int internals::calc_size(unsigned int) {
    return 4;
}

unsigned int internals::calc_size(std::string &a) {
    return 4 + (unsigned int) a.length();
}

unsigned int internals::pack(unsigned char *result, char a) {
    *result = (unsigned char) a;
    return calc_size(a);
}

unsigned int internals::pack(unsigned char *result, int a) {
    utils::packi32(result, (unsigned int) a);
    return calc_size(a);
}

unsigned int internals::pack(unsigned char *result, unsigned int a) {
    utils::packi32(result, a);
    return calc_size(a);
}

unsigned int internals::pack(unsigned char *result, std::string &a) {
    utils::pack_str(result, a);
    return calc_size(a);
}

unsigned int internals::unpack(unsigned char *result, char &a) {
    a = (char) *result;
    return calc_size(a);
}

unsigned int internals::unpack(unsigned char *result, int &a) {
    a = utils::unpacki32(result);
    return calc_size(a);
}

unsigned int internals::unpack(unsigned char *result, unsigned int &a) {
    a = utils::unpacku32(result);
    return calc_size(a);
}

unsigned int internals::unpack(unsigned char *result, std::string &a) {
    a = utils::unpack_str(result);
    return calc_size(a);
}
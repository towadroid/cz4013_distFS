//
// Created by towa-ubuntu on 3/17/20.
//

#ifndef SERVER_PACKING_LOW_LEVEL_HPP
#define SERVER_PACKING_LOW_LEVEL_HPP

#include <string>

namespace internals {
    void packi16(unsigned char *buf, const unsigned short x);

    void packi32(unsigned char *buf, unsigned int x);

    void packi64(unsigned char *buf, const unsigned long long int x);

    signed int unpacki32(const unsigned char *buf);

    unsigned int unpacku32(const unsigned char *buf);

    void pack_str(unsigned char *buf, const std::string &str);

    std::string unpack_str(unsigned char *buf);

    unsigned long long int unpacku64(const unsigned char *buf);

    void pack_float(unsigned char *buf, float x);

    void pack_double(unsigned char *buf, double x);

}


#endif //SERVER_PACKING_LOW_LEVEL_HPP

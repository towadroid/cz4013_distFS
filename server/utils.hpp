//
// Created by towa-ubuntu on 2/27/20.
//

#ifndef SERVER_UTILS_HPP
#define SERVER_UTILS_HPP

/* I use pack and unpack for marshal and unmarshal respectively
 * because they are shorter to type :)
 *
 * TODO check for IEEE754 and check for sizes
 * e.g. size_t <= 4 bytes
 * sizes: https://en.cppreference.com/w/cpp/language/types
 * if(!std::numeric_limits<float>::is_iec559;) throw std::runtime_error(IEEE754_NONCOMPLIANCE);
 */

/*short int: min. 16
 * int: min. 16
 * long int: min 32
 * long long int: min. 64
 */

/*
 * Only support ASCII encoding first.
 */

#include <string>
#include <sys/socket.h>
#include <chrono>
#include "constants.hpp"

namespace utils {
    void packi16(unsigned char *buf, unsigned short int x);

    void packi32(unsigned char *buf, unsigned int x);

    signed int unpacki32(const unsigned char *buf);

    unsigned int unpacku32(const unsigned char *buf);

    void pack_str(unsigned char *buf, const std::string &str);

    std::string unpack_str(unsigned char *buf);

    void read_file_to_string(std::string path, std::string *content);

    int read_file_to_string_cached(const std::string path, BytePtr &content, int offset, int count);

    int insert_to_file(std::string path, std::string to_insert, int offset);

    int get_in_port(sockaddr_storage const *sock_storage);

    std::string get_in_addr_str(sockaddr_storage const *sock_storage);

    void ms_to_s_usec(int ms, int &s, int &usec);

    void future_duration_to_s_usec(const std::chrono::time_point<std::chrono::steady_clock> &d, int &s, int &usec);

    bool is_similar_sockaddr_storage(const sockaddr_storage &a, const sockaddr_storage &b);

}


#endif //SERVER_UTILS_HPP

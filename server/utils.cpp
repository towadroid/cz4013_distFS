//
// Created by towa-ubuntu on 2/27/20.
//

#include <cstring>
#include <sstream> //for stringstream
#include <fstream> //for ifstream
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <filesystem>
#include "spdlog/spdlog.h"
#include "utils.hpp"
#include "HelperClasses.hpp"

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


void utils::packi16(unsigned char *buf, const unsigned short int x) {
    packin<unsigned short int>(16, buf, x);
}

void utils::packi32(unsigned char *buf, const unsigned int x) {
    packin<unsigned int>(32, buf, x);
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

unsigned int utils::unpacku32(const unsigned char *buf) {
    return unpackun<unsigned int>(32, buf);
}

signed int utils::unpacki32(const unsigned char *buf) {
    return unpackin<signed int>(32, buf);
}

/** Packs a string into a buffer.
 *
 * Buffer should be large enough to hold the string and four additional bytes.
 *
 * @param[out] buf pointer to a buffer, first four bytes store the length of str
 * @param[in] str string to be packed
 */
void utils::pack_str(unsigned char *buf, const std::string &str) {
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
std::string utils::unpack_str(unsigned char *buf) {
    size_t n = unpacku32(buf);
    return std::string(reinterpret_cast<const char *>(buf + 4), n);
}

/**Reads a whole ASCII file into a string.
 *
 * See https://stackoverflow.com/a/2602258
 *
 * @param path
 * @param content
 */
void utils::read_file_to_string(const std::string path, std::string *content) {
    if (!std::filesystem::exists(path)) throw File_does_not_exist("Could not read from file", path);
    std::ifstream in(path);
    if (!in.is_open()) throw std::runtime_error("Could not open file.");
    std::stringstream buffer;
    buffer << in.rdbuf();
    try {
        content->assign(buffer.str());
    } catch (const std::length_error &e) {
        // If the resulting string length would exceed the max_size, a length_error exception is thrown.
        spdlog::error("{}; the resulting string length would exceed the max_size", e.what());
    } catch (const std::bad_alloc &e) {
        // A bad_alloc exception is thrown if the function needs to allocate storage and fails.
        spdlog::error("{}; could not allocate storage for the string!", e.what());
    }
}

/**
 *
 * @param lower[out] start from here
 * @param upper[out] read until here (exclusive)
 * @param offset
 * @param count
 * @param chunk_size
 */
void calculate_bounds(int &lower, int &upper, const int offset, const int count, const int chunk_size) {
    lower = offset - offset % chunk_size;
    int end = offset + count;
    upper = end - end % chunk_size + chunk_size;
}

int utils::read_file_to_string_cached(const std::string path, BytePtr &content, int offset, int count) {
    if (!std::filesystem::exists(path)) throw File_does_not_exist("Could not read from file", path);
    std::ifstream in(path);
    if (!in.is_open()) throw std::runtime_error("Could not open file.");
    int start, end;
    calculate_bounds(start, end, offset, count, constants::CACHE_BLOCK_SIZE);
    int size = end - start;
    content = BytePtr(new unsigned char[size]);
    in.read((char *) content.get(), size);
    int actual_read = in.gcount();
    if (start + actual_read < offset + count)
        throw Offset_out_of_range("Could not read requested file section, offset+count exceeds bounds!",
                                  start + actual_read);
    return actual_read;
}

/**Write a string into a file.
 *
 * The string is written to the location specified by offset. Offset is used relative to the beginning of the file.
 *
 * @param path path of the file
 * @param offset offset in bytes from the beginning
 * @param to_insert string to insert
 * @return 0 if successful
 */
int utils::insert_to_file(std::string path, std::string to_insert, int offset) {
    if (!std::filesystem::exists(path)) throw File_does_not_exist("Could no insert into file", path);
    std::fstream myfile(path); // std::ios::in | std::ios::out by default
    if (!myfile.is_open()) throw std::runtime_error("Could not open file.");
    //save existing old content after offset first
    std::stringstream buffer;
    buffer << myfile.rdbuf();
    std::string content_after_offset(buffer.str());
    content_after_offset.erase(0, offset);
    if (offset > content_after_offset.length() - 1)
        throw Offset_out_of_range("Could not insert into file", (unsigned int) (content_after_offset.length() - 1));
    myfile.seekp(offset, std::ios::beg);
    myfile << to_insert;
    myfile << content_after_offset;
    return 0;
}

/** Get sockaddr, IPv4 or IPv6:
 *
 * @param sa socketaddr, that contains the address
 * @return
 */
void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) return &(((struct sockaddr_in *) sa)->sin_addr);
    return &(((struct sockaddr_in6 *) sa)->sin6_addr);
}

std::string utils::get_in_addr_str(sockaddr_storage const *sock_storage) {
    char s[INET6_ADDRSTRLEN];
    inet_ntop(sock_storage->ss_family, get_in_addr((struct sockaddr *) sock_storage), s, sizeof s);
    return std::string(s);
}

/** Get port, IPv4 or IPv6:
 *
 * @param sa socketaddr, that contains the address
 * @return
 */
int utils::get_in_port(sockaddr_storage const *sock_storage) {
    if (((sockaddr *) sock_storage)->sa_family == AF_INET) return (((struct sockaddr_in *) sock_storage)->sin_port);
    return (((struct sockaddr_in6 *) sock_storage)->sin6_port);
}

void utils::ms_to_s_usec(const int ms, int &s, int &usec) {
    s = ms / 1000;
    usec = ms % 1000;
}

void utils::future_duration_to_s_usec(const std::chrono::time_point<std::chrono::steady_clock> &d, int &s, int &usec) {
    using namespace std::chrono;
    auto duration = d - steady_clock::now();
    s = (int) duration_cast<seconds>(duration).count();
    usec = (int) duration_cast<microseconds>(duration - seconds(s)).count();
}

bool utils::is_similar_sockaddr_storage(const sockaddr_storage &a, const sockaddr_storage &b) {
    bool same_name = (0 == get_in_addr_str(&a).compare(get_in_addr_str(&b)));
    return same_name && (get_in_port(&a) == get_in_port(&b));
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
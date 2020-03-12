//
// Created by towa-ubuntu on 2/27/20.
//

#include <cstring>
#include <sstream> //for stringstream
#include <fstream> //for ifstream
#include <iostream>
#include "utils.hpp"

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

/** Packs a string into a buffer.
 *
 * Buffer should be large enough to hold the string and two additional bytes.
 *
 * @param[out] buf pointer to a buffer, first two bytes store the length of str
 * @param[in] str string to be packed
 */
void utils::pack_str(unsigned char *buf, const std::string str) {
    short int len = str.length(); //returns a value of type size_t
    //we only use the lower 2 bytes
    packi16(buf, len);
    buf += 2;
    memcpy(buf, str.c_str(), len);
}

/**Reads a whole ASCII file into a string.
 *
 * See https://stackoverflow.com/a/2602258
 *
 * @param path
 * @param content
 */
void utils::read_file_to_string(const std::string path, std::string *content) {
    std::ifstream in(path);
    if(!in.is_open()) throw std::runtime_error("Could not open file. Does file exist?");
    //TODO check if file exists, probably can use std::filesystem::exists
    std::stringstream buffer;
    buffer << in.rdbuf();
    content->assign(buffer.str());
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
    std::fstream myfile(path); // std::ios::in | std::ios::out by default
    if(!myfile.is_open()) throw std::runtime_error("Could not open file. Does file exist?"); //TODO check if file exists
    //save existing old content after offset first
    std::stringstream buffer;
    buffer << myfile.rdbuf();
    std::string content_after_offset(buffer.str());
    content_after_offset.erase(0,offset);

    myfile.seekp(offset, std::ios::beg);
    myfile << to_insert;
    myfile << content_after_offset;
    return 0;
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
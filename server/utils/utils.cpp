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
#include <sys/stat.h>
#include "spdlog/spdlog.h"
#include "utils.hpp"
#include "../HelperClasses/HelperClasses.hpp"

namespace fs = std::filesystem;

/**Reads a whole ASCII file into a string.
 *
 * See https://stackoverflow.com/a/2602258
 *
 * @param path
 * @param content
 */
std::string utils::read_file_to_string(const path &path) {
    if (!std::filesystem::exists(path)) throw File_does_not_exist("Could not read from file", path);
    std::ifstream in(path);
    if (!in.is_open()) throw std::runtime_error("Could not open file.");
    std::stringstream buffer;
    buffer << in.rdbuf();
    try {
        std::string content{buffer.str()};
        return content;
    } catch (const std::length_error &e) {
        // If the resulting string length would exceed the max_size, a length_error exception is thrown.
        spdlog::error("{}; the resulting string length would exceed the max_size", e.what());
    } catch (const std::bad_alloc &e) {
        // A bad_alloc exception is thrown if the function needs to allocate storage and fails.
        spdlog::error("{}; could not allocate storage for the string!", e.what());
    }
    return std::string{};
}

/**
 *
 * @param lower[out] start from here
 * @param upper[out] read until here (exclusive)
 * @param offset
 * @param count
 * @param chunk_size
 */
void
utils::internals2::calculate_bounds(int &lower, int &upper, const int offset, const int count, const int chunk_size) {
    lower = offset - offset % chunk_size;
    int end = offset + count - 1;
    upper = end - end % chunk_size + chunk_size;
}

void utils::exists_is_file(const path &path) {
    if (!std::filesystem::exists(path)) throw File_does_not_exist("File does not exist", path);
    if (!fs::is_regular_file(path)) throw Is_not_file("Is not file", path);
}

void utils::exists_is_dir(const path &path) {
    if (!std::filesystem::exists(path)) throw File_does_not_exist("File does not exist", path);
    if (!fs::is_directory(path)) throw Is_not_directory("Is not dir", path);
}

void utils::exists_is_file_non_empty(const path &path) {
    exists_is_file(path);
    if (fs::is_empty(path)) throw File_already_empty("File already empty", path.string());
}

std::string utils::read_file_to_string_cached(const path &path, int offset, int count) {
    exists_is_file(path);
    std::ifstream in(path);
    if (!in.is_open()) throw std::runtime_error("Could not open file.");
    int start, end;
    internals2::calculate_bounds(start, end, offset, count, constants::CACHE_BLOCK_SIZE);
    int size = end - start;

    in.seekg(start);
    auto byte_content = new unsigned char[(unsigned int) size];
    in.read((char *) byte_content, size);
    int actual_read = (int) in.gcount();
    if (start + actual_read < offset + count)
        throw Offset_out_of_range("Could not read requested file section, offset+count exceeds bounds!",
                                  start + actual_read);
    std::string content = std::string{(char *) byte_content, (unsigned long) actual_read};
    delete[] byte_content;
    return content;
}

/**Write a string into a file at a specific position, previously following content is pushed back
 *
 * The string is written to the location specified by offset. Offset is used relative to the beginning of the file.
 *
 * @param path path of the file
 * @param offset offset in bytes from the beginning
 * @param to_insert string to insert
 * @return 0 if successful
 */
int utils::insert_to_file(const path &path, std::string to_insert, unsigned int offset) {
    exists_is_file(path);
    std::fstream myfile(path); // std::ios::in | std::ios::out by default
    if (!myfile.is_open()) throw std::runtime_error("Could not open file.");
    //save existing old content after offset first
    std::stringstream buffer;
    buffer << myfile.rdbuf();
    std::string content(buffer.str());
    if (offset > content.length())
        throw Offset_out_of_range("Could not insert into file", (int) content.length());
    content.erase(0, static_cast<unsigned long>(offset));
    myfile.seekp(offset, std::ios::beg);
    myfile << to_insert;
    myfile << content;
    return 0;
}

void utils::remove_content_from_file(const path &path) {
    exists_is_file_non_empty(path);

    std::ofstream ofs;
    ofs.open(path, std::ofstream::out |
                   std::ofstream::trunc); //truncate: any contents that existed in the file before it is open are discarded.
}

void utils::remove_last_char(const path &path) {
    exists_is_file_non_empty(path);
    std::string content = read_file_to_string(path);

    content.pop_back();
    std::ofstream myfile;
    myfile.open(path, std::ios::out | std::ios::trunc);
    if (!myfile.is_open()) throw std::runtime_error("Could not open file.");
    myfile << content;
}


bool utils::file_exists(const path &path) {
    return std::filesystem::exists(path);
}

void utils::create_file(const path &path) {
    if (fs::exists(path)) throw File_already_exists("Could not create file", path);
    fs::create_directories(path.parent_path());
    std::ofstream outfile(path);
}

void utils::remove_file(const path &path) {
    bool successful_remove = fs::remove(path);
    if (!successful_remove)
        throw File_does_not_exist("Could not remove file", path);
}

std::vector<fs::path> utils::get_dir_content(const path &path) {
    exists_is_dir(path);
    std::vector<fs::path> result;
    for (auto &p: fs::directory_iterator(path))
        result.push_back(p.path());
    return result;
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

std::string utils::get_in_addr_port_str(const sockaddr_storage &sock_storage) {
    return get_in_addr_str(&sock_storage).append(":").append(std::to_string(get_in_port(&sock_storage)));
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
    if (((sockaddr *) sock_storage)->sa_family == AF_INET)
        return ntohs((((struct sockaddr_in *) sock_storage)->sin_port));
    return ntohs((((struct sockaddr_in6 *) sock_storage)->sin6_port));
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

/** https://en.cppreference.com/w/cpp/language/types
 *
 * @return
 */
bool utils::is_expected_size_and_format() {
    bool result = true;
    result &= std::numeric_limits<double>::is_iec559;
    result &= std::numeric_limits<float>::is_iec559;
    result &= (4 == sizeof(int));
    result &= (8 == sizeof(long long int));
    return result;
}

/**Returns last modification time stamp of a file as Unix epoch time (seconds since 01.01.1970 GMT)
 *
 * Might later consider to use C++ filesystem to make it portable
 * @param path
 * @return
 */
int utils::get_last_mod_time(const path &path) {
    if (!std::filesystem::exists(path)) throw File_does_not_exist("Could not insert into file", path);
    struct stat attrib;
    int err = stat(path.string().c_str(), &attrib);
    if (err != 0) perror("Could not read last modification time");
    return (int) attrib.st_ctime;
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
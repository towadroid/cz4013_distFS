//
// Created by towa-ubuntu on 2/27/20.
//

#ifndef SERVER_UTILS_HPP
#define SERVER_UTILS_HPP

/* I use pack and unpack for marshal and unmarshal respectively
 * because they are shorter to type :)
 */

/*short int: min. 16
 * int: min. 16
 * long int: min 32
 * long long int: min. 64
 */

#include <string>
#include <sys/socket.h>
#include <chrono>
#include "../constants.hpp"
#include <filesystem>

using std::filesystem::path;

namespace utils {
    bool is_expected_size_and_format();

    //-------------- file operations ----------------------
    std::string read_file_to_string(const path &path);

    std::string read_file_to_string_cached(const path &path, int offset, int count);

    int insert_to_file(const path &path, std::string to_insert, unsigned int offset);

    void remove_content_from_file(const path &path);

    void remove_last_char(const path &path);

    //-------------- internet address -------------------
    int get_in_port(sockaddr_storage const *sock_storage);

    std::string get_in_addr_str(sockaddr_storage const *sock_storage);

    bool is_similar_sockaddr_storage(const sockaddr_storage &a, const sockaddr_storage &b);

    std::string get_in_addr_port_str(const sockaddr_storage &sock_storage);

    namespace internals2 {
        void calculate_bounds(int &lower, int &upper, int offset, int count, int chunk_size);
    }

    //--------------- misc ------------------------
    void ms_to_s_usec(int ms, int &s, int &usec);

    void future_duration_to_s_usec(const std::chrono::time_point<std::chrono::steady_clock> &d, int &s, int &usec);
}


#endif //SERVER_UTILS_HPP

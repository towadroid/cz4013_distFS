//
// Created by Tobias on 23.02.2020.
//

#ifndef SERVER_CONSTANTS_HPP
#define SERVER_CONSTANTS_HPP

#include <string>
#include <unordered_map>
#include <chrono>
#include <netinet/in.h>
#include <memory>

namespace constants {
    // ---------------- invocation semantics ---------------------
    constexpr int ATLEAST = 0;
    constexpr int ATMOST = 1;

    // ---------------- Packet constants -------------------------
    constexpr int HEADER_SIZE = 12;
    /// this is the overall max packet size, including header
    constexpr int MAX_PACKET_SIZE = 256;
    constexpr int MAX_CONTENT_SIZE = MAX_PACKET_SIZE - HEADER_SIZE;

    constexpr int CACHE_BLOCK_SIZE = 100;

    /// timeout time waiting for ack's in ms
    constexpr int ACK_TIMEOUT = 10000; //TODO maybe higher value?

    /// time to wait for next fragment in ms
    constexpr int FRAG_TIMEOUT = 10000;

    //---------- status constants --------------------------------
    constexpr int SUCCESS = 0;
    constexpr int FILE_DOES_NOT_EXIST = 1;
    constexpr int OFFSET_OUT_OF_BOUNDS = 2;
    constexpr int FILE_ALREADY_EMPTY = 3;
    constexpr int FILE_WAS_MODIFIED = 4;
    constexpr int SERVER_BUSY = 11;

    //----------- service constants -----------------------------
    enum class Service_type : int {
        read,
        insert,
        register_client,
        remove_content,
        remove_last_char,
        file_mod_time,
        ack_recvd_reply,
    };

    const std::unordered_map<int, Service_type> service_codes = {
            {1, Service_type::read},
            {2, Service_type::insert},
            {3, Service_type::register_client},
            {4, Service_type::remove_content},
            {5, Service_type::remove_last_char},
            {6, Service_type::file_mod_time},
            {7, Service_type::ack_recvd_reply}
    };

    const std::string IEEE754_NONCOMPLIANCE("Not IEC559/IEEE754 compliant, cannot (un-)marshal data");

    /** Path for files in the "file system".
     *
     * The working directory per default is "..."/cz4013_distFS/server/cmake-build-debug
     * Can change by setting "working directory" by editing run configuration
     */
    const std::string FILE_DIR_PATH("../filesystem_root/");
}

typedef std::shared_ptr<unsigned char[]> BytePtr;
///0 shared pointer to message, 2 size of message
typedef std::pair<BytePtr, size_t> MessagePair;
typedef std::chrono::time_point<std::chrono::steady_clock> TimeStamp;
/// 0 timeout time, 1 clientaddr, 2 requestID
typedef std::tuple<TimeStamp, sockaddr_storage, unsigned int> TimeoutElement;


#endif //SERVER_CONSTANTS_HPP

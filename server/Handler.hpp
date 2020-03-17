//
// Created by towa-ubuntu on 3/12/20.
//

#ifndef SERVER_HANDLER_HPP
#define SERVER_HANDLER_HPP

#include "constants.hpp"
#include "UdpServer_linux.hpp"
#include "MonitoringClient.hpp"
#include "HelperClasses.hpp"
#include <type_traits>
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <queue>
#include <chrono>
#include <tuple>
#include <filesystem>

using constants::Service_type;
using std::unordered_map;
using std::string;
using std::filesystem::path;

using constants::MAX_CONTENT_SIZE;
using constants::MAX_PACKET_SIZE;
using constants::HEADER_SIZE;

constexpr int DID_NOT_RECEIVE = -10;

class Handler {
public:
    void service(Service_type service_type, const UdpServer_linux &server, unsigned char *message);

private:
    unordered_map<string, std::vector<MonitoringClient>> registered_clients{};

    /** field to store messages, if need to retrieve later
     *
     * first map for client address; second map for requestID
     * final value is the pair of message (ptr) + len
     */
    unordered_map<sockaddr_storage, unordered_map<int, MessagePair>, SockaddrStor_Hasher, SockaddrStor_Equal> stored_messages{};

    std::priority_queue<TimeoutElement, std::vector<TimeoutElement>, TimeoutElement_less> timeout_times{};

    unsigned char buffer_mem[constants::MAX_PACKET_SIZE];

    void service_read(unsigned char *message, BytePtr &raw_result, int &result_length);

    void service_register_client(unsigned char *message, BytePtr &raw_result, int &result_length,
                                 const sockaddr_storage &client);

    void
    store_message(const sockaddr_storage &client_address, const int requestID, const BytePtr message, const size_t len);

    void notify_registered_clients(const string &filename, const UdpServer_linux &server);

    bool is_duplicate_request(const sockaddr_storage *client_address, int requestID);

    void receive_handle_message(UdpServer_linux &server, int semantic);

    void send_complete_message(const UdpServer_linux &server, const unsigned char *raw_content_buf, size_t len,
                               int requestID,
                               const sockaddr_storage &receiver);

    MessagePair retrieve_stored_message(const sockaddr_storage &client_address, int requestID);

    int receive_specific_packet(UdpServer_linux &server, int semantic, const sockaddr_storage *const exp_address,
                                int exp_requestID, int exp_fragment_no, unsigned char *dest_buf,
                                const TimeStamp *const timeout_time);

    bool is_ACK(); //TODO implement

    void handle_ACK(); //TODO implement
    void unpack_header(const unsigned char *buf, int &requestID, int &overall_size, int &fragment_no);

    int receive_specific_packet(UdpServer_linux &server, int semantic, const sockaddr_storage *const exp_address,
                                int exp_requestID, int exp_fragment_no, unsigned char *dest_buf, int timeout_ms);

    void service_insert(unsigned char *message, BytePtr &raw_result, int &result_length);
};


#endif //SERVER_HANDLER_HPP

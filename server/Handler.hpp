//
// Created by towa-ubuntu on 3/12/20.
//

#ifndef SERVER_HANDLER_HPP
#define SERVER_HANDLER_HPP

#include "constants.hpp"
#include "UdpServer_linux.hpp"
#include "MonitoringClient.hpp"
#include <type_traits>
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <queue>
#include <chrono>
#include <tuple>

using constants::Service_type;
using std::unordered_map;
using std::string;

using constants::MAX_CONTENT_SIZE;
using constants::MAX_PACKET_SIZE;
using constants::HEADER_SIZE;

constexpr int DID_NOT_RECEIVE = -10;

typedef std::shared_ptr<unsigned char[]> BytePtr;
typedef std::pair<BytePtr, size_t> MessagePair;
typedef std::chrono::time_point<std::chrono::steady_clock> TimeStamp;
typedef std::tuple<TimeStamp, string, int, int> TimeoutElement;

class Handler {
public:
    void service(Service_type service_type, const UdpServer_linux &server, unsigned char *message);

private:
    unordered_map<string, std::vector<MonitoringClient>> registered_clients{};

    /** field to store messages, if need to retrieve later
     *
     * first map for inet address; second map for port number; third map for requestID
     * final value is the pair of message (ptr) + len
     */
    unordered_map<string, unordered_map<int, unordered_map<int, MessagePair>>> stored_messages{};

    std::priority_queue<TimeoutElement> timeout_times{};

    unsigned char buffer_mem[constants::MAX_PACKET_SIZE];

    /**
     * overload function by the type integral_constant, each enum matches a type
     * this ensures function names can all be the same
     * advantage: services can have arbitrary names, only dependent on enum
     *
     * @param server
     * @param message
     */
    void
    exec_service(std::integral_constant<Service_type, Service_type::not_a_service>, const UdpServer_linux &server,
                 unsigned char *message);

    void exec_service(std::integral_constant<Service_type, Service_type::service1>, const UdpServer_linux &server,
                      unsigned char *message);

    void
    exec_service(std::integral_constant<Service_type, Service_type::register_client>, const UdpServer_linux &server,
                 unsigned char *message);

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
                                int exp_requestID, int exp_fragment_no, unsigned char *dest_buf,
                                int timeout_ms);
};


#endif //SERVER_HANDLER_HPP

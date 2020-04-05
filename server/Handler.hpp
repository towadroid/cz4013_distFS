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
    virtual ~Handler();

    virtual void service(Service_type service_type, const UdpServer_linux &server, unsigned char *complete_raw_content,
                         BytePtr &raw_reply, unsigned int &raw_reply_length);

    void receive_handle_message(UdpServer_linux &server, int semantic);

private:
    unordered_map<string, std::vector<MonitoringClient>> registered_clients{};

    /** field to store messages, if need to retrieve later
     *
     * first map for client address; second map for requestID
     * final value is the pair of message (ptr) + len
     */
    unordered_map<sockaddr_storage, unordered_map<unsigned int, MessagePair>, SockaddrStor_Hasher, SockaddrStor_Equal> stored_messages{};

    ///priority queue to track when to resend messages that where not acknowledged
    std::priority_queue<TimeoutElement, std::vector<TimeoutElement>, TimeoutElement_less> timeout_times{};

    //----------------------- Services offered -----------------------
    void service_read(unsigned char *message, BytePtr &raw_result, unsigned int &result_length);

    void
    service_insert(unsigned char *message, BytePtr &raw_result, unsigned int &result_length, std::string &path_string);

    void service_register_client(unsigned char *message, BytePtr &raw_result, unsigned int &result_length,
                                 const sockaddr_storage &client);

    void service_remove_all_content(unsigned char *message, BytePtr &raw_result, unsigned int &result_length,
                                    string &path_string);

    void service_remove_last_char(unsigned char *message, BytePtr &raw_result, unsigned int &result_length,
                                  string &path_string);

    void service_last_mod_time(unsigned char *message, BytePtr &raw_result, unsigned int &result_length);

    void resend_timeouted_messages(UdpServer_linux &server);

    //------------------------ stored_messages interaction ------------
    void store_message(const sockaddr_storage &client_address, unsigned int requestID, BytePtr message, size_t len);


    MessagePair retrieve_stored_message(const sockaddr_storage &client_address, unsigned int requestID);

    void notify_registered_clients(const string &filename, const UdpServer_linux &server);

    bool is_duplicate_request(const sockaddr_storage *client_address, const unsigned int requestID);

    bool is_ACK(unsigned char *raw_content);

    void handle_ACK(unsigned int requestID, const sockaddr_storage &client);

    bool handle_ack_or_duplicate(UdpServer_linux &server, unsigned char *buffer, const sockaddr_storage &client_address,
                                 unsigned int requestID, unsigned int fragment_no);

    void
    unpack_header(unsigned char *buf, unsigned int &requestID, unsigned int &overall_size, unsigned int &fragment_no);

    //----------------------- server interaction ------------------------
    virtual void send_complete_message(const UdpServer_linux &server, unsigned char *const raw_content_buf, size_t len,
                                       unsigned int requestID, const sockaddr_storage &receiver);

    int receive_specific_packet(UdpServer_linux &server, int semantic, const sockaddr_storage *const exp_address,
                                unsigned int exp_requestID, unsigned int exp_fragment_no, unsigned char *dest_buf,
                                TimeStamp *const timeout_time);

    int receive_specific_packet(UdpServer_linux &server, int semantic, const sockaddr_storage *const exp_address,
                                unsigned int exp_requestID, unsigned int exp_fragment_no, unsigned char *dest_buf,
                                int timeout_ms);


    bool check_if_correct_packet(UdpServer_linux &server, const sockaddr_storage *const exp_address,
                                 const sockaddr_storage &client_address, unsigned int exp_requestID,
                                 unsigned int requestID,
                                 unsigned int exp_fragment_no, unsigned int fragment_no, unsigned char *dest_buf,
                                 unsigned char *recv_buffer, int n);
};


#endif //SERVER_HANDLER_HPP

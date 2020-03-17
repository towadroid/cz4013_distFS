//
// Created by towa-ubuntu on 3/12/20.
//

#include "Handler.hpp"
#include "utils/utils.hpp"
#include "utils/packing.hpp"
#include <iostream>
#include <stdexcept>
#include <cstring> //for memcpy
#include "spdlog/spdlog.h"

using constants::Service_type;

/** Calls the function execution the service associated with the service type
 *
 * @param[in] service_type enum specifying which service to execute
 */
void Handler::service(Service_type service_type, const UdpServer_linux &server, unsigned char *message) {
    BytePtr raw_content;
    int raw_length;
    switch (service_type) {
        case Service_type::read: {
            service_read(message, raw_content, raw_length);
            break;
        }
        case Service_type::register_client: {
            service_register_client(message, raw_content, raw_length, server.get_client_address());
            break;
        }
        case Service_type::insert: {
            service_insert(message, raw_content, raw_length);
            break;
        }
        default: {
            spdlog::error("Service could not be identified!");
        }
    }
    //TODO send reply
}

void Handler::service_read(unsigned char *message, BytePtr &raw_result, int &result_length) {
    int offset, count;
    std::string path_string;
    utils::unpack(message, path_string, offset, count);
    path path(constants::FILE_DIR_PATH + path_string);

    try {
        std::string file_content;
        utils::read_file_to_string_cached(path, file_content, offset, count);
        result_length = (int) utils::pack(raw_result, file_content);
    } catch (const File_does_not_exist &e) {
        //TODO
    } catch (const Offset_out_of_range &e) {

    }
}

void Handler::service_insert(unsigned char *message, BytePtr &raw_result, int &result_length) {
    int offset;
    std::string path_string, to_write;
    utils::unpack(message, path_string, offset, to_write);
    path path(constants::FILE_DIR_PATH + path_string);

    string content_to_write{"Test insert"};
    try {
        utils::insert_to_file(path, content_to_write, offset);
        //TODO send ack
    } catch (const File_does_not_exist &e) {

    } catch (const Offset_out_of_range &e) {

    }
}

void
Handler::service_register_client(unsigned char *message, BytePtr &raw_result, int &result_length,
                                 const sockaddr_storage &client) {
    std::string path_string;
    int mon_interval;
    utils::unpack(message, path_string, mon_interval);
    registered_clients[path_string].push_back(MonitoringClient{client, mon_interval});
    //TODO reply some message
}

void Handler::notify_registered_clients(const std::string &filename, const UdpServer_linux &server) {
    std::vector<MonitoringClient> file_reg_clients;
    try { file_reg_clients = registered_clients.at(filename); }
    catch (const std::out_of_range &oor) {} // there is no entry for this filename => nothing to doo

    for (auto it = file_reg_clients.begin(); it != file_reg_clients.end(); ++it) {
        if (it->expired()) file_reg_clients.erase(it);
        else {
            //TODO send notification message to client
            fprintf(stdout, "Send notification to client");
        }
    }

}

void
Handler::store_message(const sockaddr_storage &client_address, const int requestID, const BytePtr message,
                       const size_t len) {
    stored_messages[client_address][requestID] = MessagePair{message, len};
}

bool Handler::is_duplicate_request(const sockaddr_storage *client_address, const int requestID) {
    if (0 == stored_messages.count(*client_address)) return false;

    return !(0 == stored_messages[*client_address].count(requestID));
}

void Handler::receive_handle_message(UdpServer_linux &server, const int semantic) {
    while (!timeout_times.empty()) {
        TimeoutElement te = timeout_times.top();
        if (std::get<0>(te) < std::chrono::steady_clock::now()) {
            //has timed out
            using std::get;
            MessagePair pair = stored_messages[get<1>(te)][get<2>(te)];
            //send_complete_message(server, pair.first.get(), pair.second, get<3>(te), )
            timeout_times.pop();
            std::get<0>(te) = std::chrono::steady_clock::now() + std::chrono::milliseconds(constants::ACK_TIMEOUT);
            timeout_times.push(te);
        }
    }
    auto next_timeout = std::get<0>(timeout_times.top());

    unsigned char recvd_msg[MAX_PACKET_SIZE];

    receive_specific_packet(server, semantic, nullptr, 0, 0, recvd_msg, &next_timeout);

    int requestID, overall_size, fragment_no;
    unpack_header(recvd_msg, requestID, overall_size, fragment_no);
    const sockaddr_storage &client_address = server.get_client_address();
    BytePtr message = BytePtr(new unsigned char[(unsigned int) overall_size]);
    int fragments_expected = overall_size % MAX_CONTENT_SIZE + 1;
    int cur_frag_no = 0;
    size_t cur_len = MAX_PACKET_SIZE;
    do {
        if (cur_frag_no == fragments_expected - 1) cur_len = overall_size % MAX_CONTENT_SIZE;
        memcpy(&message[cur_frag_no * MAX_CONTENT_SIZE], recvd_msg + HEADER_SIZE, cur_len);
        ++cur_frag_no;
        receive_specific_packet(server, semantic, &client_address, requestID, fragment_no, recvd_msg,
                                constants::FRAG_TIMEOUT);
    } while (cur_frag_no < fragments_expected);

    //unsigned int service_no = utils::unpacku32(message.get());
    //TODO
    // service(constants::service_codes.at(service_no), server, message.get());

    if (semantic == constants::ATMOST) store_message(client_address, requestID, message, (unsigned int) overall_size);
}

/** Given a buffer for raw content to send and its length, send the eventuallz fragmented message
 *
 * @param server
 * @param raw_content_buf
 * @param len
 * @param requestID
 * @param receiver
 */
void Handler::send_complete_message(const UdpServer_linux &server, const unsigned char *raw_content_buf, size_t len,
                                    int requestID, const sockaddr_storage &receiver) {
    BytePtr result;
    BytePtr partial_header;
    unsigned int partial_header_len = utils::pack(partial_header, requestID, (int) len);

    size_t cur_content_len = MAX_CONTENT_SIZE;
    unsigned int fragments_to_send = len % MAX_CONTENT_SIZE + 1;
    for (unsigned int cur_frag_no = 0; cur_frag_no < fragments_to_send; ++cur_frag_no) {
        if (cur_frag_no == fragments_to_send - 1) cur_content_len = len % MAX_CONTENT_SIZE;

        utils::pack(result, partial_header_len, partial_header.get(), cur_frag_no, (unsigned int) cur_content_len,
                    raw_content_buf + cur_frag_no * MAX_CONTENT_SIZE);
        server.send_msg(result.get(), cur_content_len + MAX_PACKET_SIZE, &receiver);
    }
}

MessagePair Handler::retrieve_stored_message(const sockaddr_storage &client_address, int requestID) {
    return stored_messages[client_address][requestID];
}

/** Receive only a specific packet (Client, port, requestID, fragment_no)
 *
 * Handles incoming ACK's and duplicate requests
 * returns
 *
 * @param server
 * @param semantic
 * @param exp_address[in] the client we want to receive from
 *          if nullptr: receive from any, then exp_requestID, exp_fragment_no and exp_len are ignored
 * @param exp_requestID
 * @param exp_fragment_no
 * @param exp_len
 * @param timeout_time[in] time, until when we should wait for the spcecific packet, if nullptr -> wait forever
 * @return status code
 */
int Handler::receive_specific_packet(UdpServer_linux &server, int semantic, const sockaddr_storage *const exp_address,
                                     int exp_requestID, int exp_fragment_no, unsigned char *dest_buf,
                                     const TimeStamp *const timeout_time) {
    while (std::chrono::steady_clock::now() > *timeout_time) {
        int s, usec;
        utils::future_duration_to_s_usec(*timeout_time, s, usec);
        int n = server.receive_msg(buffer_mem, s, usec);
        if (n > 0) {
            unsigned char *cur = buffer_mem;
            int requestID, overall_size, fragment_no;
            unpack_header(buffer_mem, requestID, overall_size, fragment_no);
            const sockaddr_storage &client_address = server.get_client_address();

            if (semantic == constants::ATMOST) {
                if (is_ACK()) {
                    handle_ACK();
                    continue;
                }
                if (is_duplicate_request(&client_address, requestID) && 0 == fragment_no) {
                    spdlog::info("Duplicate request, resend message {} to {}:{}", requestID,
                                 utils::get_in_addr_str(&client_address),
                                 utils::get_in_port(&client_address));
                    MessagePair msg_pair = retrieve_stored_message(client_address, requestID);
                    send_complete_message(server, msg_pair.first.get(), msg_pair.second, requestID, client_address);
                    continue;
                }
            }

            //check if it is the packet we want
            if (nullptr == exp_address || utils::is_similar_sockaddr_storage(*exp_address, client_address)) {
                //correct sender
                if (exp_requestID == requestID && exp_fragment_no == fragment_no) {
                    //correct packet
                    memcpy(dest_buf, buffer_mem, n);
                    return n;
                } //ignore wrong packet from correct sender
            } else {
                // wrong sender
                //TODO tell sender that server is busy
                continue;
            }
        } else if (TIMEOUT == n) {
            return DID_NOT_RECEIVE;
        }
    }
    return DID_NOT_RECEIVE;
}

int Handler::receive_specific_packet(UdpServer_linux &server, int semantic, const sockaddr_storage *const exp_address,
                                     int exp_requestID, int exp_fragment_no, unsigned char *dest_buf,
                                     int timeout_ms) {
    auto timeout_time = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeout_ms);
    return receive_specific_packet(server, semantic, exp_address, exp_requestID, exp_fragment_no, dest_buf,
                                   &timeout_time);
}

bool Handler::is_ACK() {
    return false;
}

void Handler::handle_ACK() {

}

void Handler::unpack_header(const unsigned char *buf, int &requestID, int &overall_size, int &fragment_no) {
    utils::unpack(buf, requestID, overall_size, fragment_no);
}

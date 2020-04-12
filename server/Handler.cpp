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
#include "spdlog/fmt/bin_to_hex.h"
#include "utils/logger_helper.hpp"

using constants::Service_type;
namespace fs = std::filesystem;

/** Calls the function executing the service associated with the service type
 *
 * @param[in] service_type enum specifying which service to execute
 */
void Handler::service(Service_type service_type, UdpServer_linux &server, unsigned char *complete_raw_content,
                      BytePtr &raw_reply, unsigned int &raw_reply_length, const sockaddr_storage &client_address,
                      unsigned int requestID) {
    unsigned char *raw_content_wo_servno = complete_raw_content + 4;
    std::string path_string{""};
    switch (service_type) {
        case Service_type::read: {
            service_read(raw_content_wo_servno, raw_reply, raw_reply_length);
            break;
        }
        case Service_type::register_client: {
            service_register_client(raw_content_wo_servno, raw_reply, raw_reply_length, client_address, requestID);
            break;
        }
        case Service_type::insert: {
            service_insert(raw_content_wo_servno, raw_reply, raw_reply_length, path_string);
            break;
        }
        case Service_type::remove_content: {
            service_remove_all_content(raw_content_wo_servno, raw_reply, raw_reply_length, path_string);
            break;
        }
        case Service_type::remove_last_char: {
            service_remove_last_char(raw_content_wo_servno, raw_reply, raw_reply_length, path_string);
            break;
        }
        case Service_type::file_mod_time: {
            service_last_mod_time(raw_content_wo_servno, raw_reply, raw_reply_length);
            break;
        }
        case Service_type::ack_recvd_reply: {
            spdlog::warn(
                    "Tried to invoke ack_recvd_reply service, this should not happen because acks are processed earlier!");
            break;
        }
        case Service_type::create_file: {
            service_create_file(raw_content_wo_servno, raw_reply, raw_reply_length);
            break;
        }
        case Service_type::remove_file: {
            service_remove_file(raw_content_wo_servno, raw_reply, raw_reply_length, path_string);
            break;
        }
        case Service_type::list_dir: {
            service_list_dir_content(raw_content_wo_servno, raw_reply, raw_reply_length);
            break;
        }
        default: {
            spdlog::error("Service could not be identified!");
        }
    }
    if (!path_string.empty()) notify_registered_clients(path_string, server);
}

void Handler::service_read(unsigned char *message, BytePtr &raw_result, unsigned int &result_length) {
    int offset, count;
    std::string path_string;
    utils::unpack(message, path_string, offset, count);
    path path(constants::FILE_DIR_PATH + path_string);

    try {
        std::string file_content = utils::read_file_to_string_cached(path, offset, count);
        result_length = utils::pack(raw_result, constants::SUCCESS, file_content);
    } catch (const File_does_not_exist &e) {
        result_length = utils::pack(raw_result, constants::FILE_DOES_NOT_EXIST);
    } catch (const Offset_out_of_range &e) {
        result_length = utils::pack(raw_result, constants::OFFSET_OUT_OF_BOUNDS);
    } catch (const Is_not_file &e) {
        result_length = utils::pack(raw_result, constants::IS_NOT_FILE);
    }
}

/**
 *
 * @param message
 * @param raw_result
 * @param result_length
 * @param path_string[out] path_string to modified path, empty string if not successful
 */
void
Handler::service_insert(unsigned char *message, BytePtr &raw_result, unsigned int &result_length,
                        std::string &path_string) {
    int offset;
    std::string content_to_write;
    utils::unpack(message, path_string, offset, content_to_write);
    path path(constants::FILE_DIR_PATH + path_string);

    try {
        utils::insert_to_file(path, content_to_write, offset);
        result_length = utils::pack(raw_result, constants::SUCCESS);
    } catch (const File_does_not_exist &e) {
        result_length = utils::pack(raw_result, constants::FILE_DOES_NOT_EXIST);
        path_string = "";
    } catch (const Offset_out_of_range &e) {
        result_length = utils::pack(raw_result, constants::OFFSET_OUT_OF_BOUNDS);
        path_string = "";
    } catch (const Is_not_file &e) {
        result_length = utils::pack(raw_result, constants::IS_NOT_FILE);
        path_string = "";
    }
}

void
Handler::service_register_client(unsigned char *message, BytePtr &raw_result, unsigned int &result_length,
                                 const sockaddr_storage &client, unsigned int requestID) {
    std::string path_string;
    int mon_interval;
    utils::unpack(message, path_string, mon_interval);
    try {
        path path = constants::FILE_DIR_PATH + path_string;
        utils::exists_is_file(path);
        std::string relative_path = fs::relative(path, constants::FILE_DIR_PATH).string();
        registered_clients[relative_path].push_back(MonitoringClient{client, mon_interval, requestID});
        result_length = utils::pack(raw_result, constants::SUCCESS);
    } catch (const File_does_not_exist &e) {
        result_length = utils::pack(raw_result, constants::FILE_DOES_NOT_EXIST);
    } catch (const Is_not_file &e) {
        result_length = utils::pack(raw_result, constants::IS_NOT_FILE);
    }
}

/**
 *
 * @param message
 * @param raw_result
 * @param result_length
 * @param path_string[out] path_string to modified path, empty string if not successful / no modification
 */
void
Handler::service_remove_all_content(unsigned char *message, BytePtr &raw_result, unsigned int &result_length,
                                    std::string &path_string) {
    utils::unpack(message, path_string);
    try {
        utils::remove_content_from_file(path{constants::FILE_DIR_PATH + path_string});
        result_length = utils::pack(raw_result, constants::SUCCESS);
    } catch (const File_does_not_exist &e) {
        result_length = utils::pack(raw_result, constants::FILE_DOES_NOT_EXIST);
        path_string = "";
    } catch (const File_already_empty &e) {
        result_length = utils::pack(raw_result, constants::SUCCESS);
        path_string = "";
    } catch (const Is_not_file &e) {
        result_length = utils::pack(raw_result, constants::IS_NOT_FILE);
        path_string = "";
    }
}

/**
 *
 * @param message
 * @param raw_result
 * @param result_length
 * @param path_string[out] path_string to modified path, empty string if not successful / no modification
 */
void
Handler::service_remove_last_char(unsigned char *message, BytePtr &raw_result, unsigned int &result_length,
                                  std::string &path_string) {
    utils::unpack(message, path_string);
    try {
        utils::remove_last_char(path{constants::FILE_DIR_PATH + path_string});
        result_length = utils::pack(raw_result, constants::SUCCESS);
    } catch (const File_does_not_exist &e) {
        result_length = utils::pack(raw_result, constants::FILE_DOES_NOT_EXIST);
        path_string = "";
    } catch (const File_already_empty &e) {
        result_length = utils::pack(raw_result, constants::FILE_ALREADY_EMPTY);
        path_string = "";
    } catch (const Is_not_file &e) {
        result_length = utils::pack(raw_result, constants::IS_NOT_FILE);
        path_string = "";
    }
}

void Handler::service_last_mod_time(unsigned char *message, BytePtr &raw_result, unsigned int &result_length) {
    std::string path_string;
    utils::unpack(message, path_string);
    try {
        int last_m_time = utils::get_last_mod_time(path{constants::FILE_DIR_PATH + path_string});
        result_length = utils::pack(raw_result, constants::SUCCESS, last_m_time);
    } catch (const File_does_not_exist &e) {
        result_length = utils::pack(raw_result, constants::FILE_DOES_NOT_EXIST);
    } catch (const Is_not_file &e) {
        result_length = utils::pack(raw_result, constants::IS_NOT_FILE);
    }

}

void Handler::service_create_file(unsigned char *message, BytePtr &raw_result, unsigned int &result_length) {
    std::string path_string;
    utils::unpack(message, path_string);
    try {
        utils::create_file(constants::FILE_DIR_PATH + path_string);
        result_length = utils::pack(raw_result, constants::SUCCESS);
    } catch (const File_already_exists &e) {
        result_length = utils::pack(raw_result, constants::FILE_ALREADY_EXISTS);
    }
}

void Handler::service_remove_file(unsigned char *message, BytePtr &raw_result, unsigned int &result_length,
                                  std::string &path_string) {
    utils::unpack(message, path_string);
    try {
        utils::remove_file(constants::FILE_DIR_PATH + path_string);
        result_length = utils::pack(raw_result, constants::SUCCESS);
    } catch (const File_does_not_exist &e) {
        result_length = utils::pack(raw_result, constants::FILE_DOES_NOT_EXIST);
        path_string = "";
    }
}

void Handler::service_list_dir_content(unsigned char *message, BytePtr &raw_result, unsigned int &result_length) {
    std::string path_string;
    utils::unpack(message, path_string);
    try {
        std::vector<std::filesystem::path> content = utils::get_dir_content(constants::FILE_DIR_PATH);
        result_length = utils::pack(raw_result, (unsigned int) content.size());
        for (auto &it : content) {
            if (std::filesystem::is_directory(it))
                result_length = utils::pack(raw_result, result_length, raw_result.get(), constants::IS_DIR,
                                            it.filename().string());
            else if (std::filesystem::is_regular_file(it))
                result_length = utils::pack(raw_result, result_length, raw_result.get(), constants::IS_FILE,
                                            it.filename().string());
            else
                throw std::runtime_error("Unknown file type!");
        }
    } catch (const File_does_not_exist &e) {
        result_length = utils::pack(raw_result, constants::FILE_DOES_NOT_EXIST);
        path_string = "";
    } catch (const Is_not_directory &e) {
        result_length = utils::pack(raw_result, constants::IS_NOT_DIRECTORY);
        path_string = "";
    }
}


void Handler::notify_registered_clients(const std::string &filename, UdpServer_linux &server) {
    std::vector<MonitoringClient> file_reg_clients;
    fs::path path{constants::FILE_DIR_PATH + filename};
    std::string relative_path = fs::relative(path, constants::FILE_DIR_PATH).string();
    try { file_reg_clients = registered_clients.at(relative_path); }
    catch (const std::out_of_range &oor) {} // there is no entry for this filename => nothing to doo

    for (auto it = file_reg_clients.begin(); it != file_reg_clients.end();) {
        if (it->expired()) it = file_reg_clients.erase(it);
        else {
            BytePtr raw_content;
            unsigned int raw_length;
            if (!utils::file_exists(path)) {
                raw_length = utils::pack(raw_content, constants::FILE_DOES_NOT_EXIST, relative_path);
                spdlog::info("Send notification to {}, that file '{}' was deleted",
                             utils::get_in_addr_port_str(it->getAddress()), filename);
            } else {
                std::string file_content = utils::read_file_to_string(path);
                raw_length = utils::pack(raw_content, constants::FILE_WAS_MODIFIED, relative_path, file_content);
                spdlog::info("Send notification to {}, that file '{}' has changed",
                             utils::get_in_addr_port_str(it->getAddress()), filename);
            }

            bool success = send_complete_message(server, raw_content.get(), raw_length, it->getRequestId(),
                                                 it->getAddress());
            if (!success) spdlog::warn("Simulated Failure: Packets were dropped during the former action!");

            ++it;
        }
    }

}

void
Handler::store_message(const sockaddr_storage &client_address, const unsigned int requestID, const BytePtr message,
                       const size_t len) {
    stored_messages[client_address][requestID] = MessagePair{message, len};
    spdlog::info("Message #{} of {} added to stored list.",
                 requestID, utils::get_in_addr_port_str(client_address));
}

bool Handler::is_duplicate_request(const sockaddr_storage *client_address, const unsigned int requestID) {
    if (0 == stored_messages.count(*client_address)) return false;

    return !(0 == stored_messages[*client_address].count(requestID));
}

void Handler::resend_timeouted_messages(UdpServer_linux &server) {
    while (!timeout_times.empty()) {
        TimeoutElement te = timeout_times.top();
        if (std::get<0>(te) < std::chrono::steady_clock::now()) {
            //has timed out
            using std::get;
            try {
                MessagePair pair = stored_messages.at(get<1>(te)).at(get<2>(te));
                bool success = send_complete_message(server, pair.first.get(), pair.second, get<2>(te), get<1>(te));
                if (success)
                    spdlog::info("Message #{} from {} has timed out and is being resent.",
                                 get<2>(te), utils::get_in_addr_port_str(get<1>(te)));
                else
                    spdlog::warn(
                            "Simulated failure: Message #{} from {} has timed out but was not successfully resent.",
                            get<2>(te), utils::get_in_addr_port_str(get<1>(te)));
                timeout_times.pop();
                std::get<0>(te) = std::chrono::steady_clock::now() + std::chrono::milliseconds(constants::ACK_TIMEOUT);
                timeout_times.push(te);
            } catch (const std::out_of_range &e) {
                //if there is no entry in stored_messages then it has been ack'ed, so remove timeout
                timeout_times.pop();
            }


        } else break;
    }
}

void Handler::receive_handle_message(UdpServer_linux &server, const int semantic) {
    resend_timeouted_messages(server);

    ReceiveSpecifier recv_spec{};

    if (!timeout_times.empty()) {
        auto next_timeout = std::get<0>(timeout_times.top());
        recv_spec = ReceiveSpecifier(next_timeout);
    } else recv_spec = ReceiveSpecifier();


    unsigned char recvd_msg[MAX_PACKET_SIZE];

    if (DID_NOT_RECEIVE == receive_specific_packet(server, semantic, recv_spec, recvd_msg)) {
        return;
    }

    unsigned int requestID, overall_size, fragment_no;
    unpack_header(recvd_msg, requestID, overall_size, fragment_no);
    const sockaddr_storage &client_address = recv_spec.getExpAddress();

    BytePtr raw_content = BytePtr(new unsigned char[overall_size]);
    unsigned int fragments_expected = overall_size / MAX_CONTENT_SIZE + 1;
    unsigned int cur_frag_no = 0;
    size_t cur_len = MAX_CONTENT_SIZE;
    do {
        if (cur_frag_no + 1 == fragments_expected) cur_len = overall_size % MAX_CONTENT_SIZE;
        memcpy(&raw_content[cur_frag_no * MAX_CONTENT_SIZE], recvd_msg + HEADER_SIZE, cur_len);
        ++cur_frag_no;
        if (cur_frag_no < fragments_expected) {
            recv_spec = ReceiveSpecifier(constants::FRAG_TIMEOUT, client_address, requestID, cur_frag_no);
            if (DID_NOT_RECEIVE == receive_specific_packet(server, semantic, recv_spec, recvd_msg)) {
                spdlog::warn("Did not receive expected packet, abort receive operation.");
                //TODO wrong packet received
                return;
            }
        }
    } while (cur_frag_no < fragments_expected);

    BytePtr raw_reply;
    unsigned int raw_reply_length, service_no;
    utils::unpack(raw_content.get(), service_no);

    spdlog::info("Received message #{} from {} with raw content size {} bytes",
                 requestID, utils::get_in_addr_port_str(client_address), overall_size);
    utils::print_request(raw_content.get());

    service(constants::service_codes.at((int) service_no), server, raw_content.get(), raw_reply, raw_reply_length,
            client_address, 0);

    bool success = send_complete_message(server, raw_reply.get(), raw_reply_length, requestID, client_address);
    if (success)
        spdlog::info("Reply for #{} sent to {} with raw content size {} bytes",
                     requestID, utils::get_in_addr_port_str(client_address), raw_reply_length);
    else
        spdlog::warn("Simulated failure: Reply for #{} not successfully sent to {}", requestID,
                     utils::get_in_addr_port_str(client_address));
    utils::print_reply(service_no, raw_reply.get(), raw_reply_length);

    if (semantic == constants::ATMOST) {
        store_message(client_address, requestID, raw_reply, raw_reply_length);
        using std::chrono::steady_clock;
        TimeoutElement timeout_elem{steady_clock::now() + std::chrono::milliseconds(constants::ACK_TIMEOUT),
                                    client_address, requestID};
        timeout_times.push(timeout_elem);
    }
}

/** Given a buffer for raw content to send and its length, send the eventually fragmented message
 *
 * @param server
 * @param raw_content_buf
 * @param len
 * @param requestID
 * @param receiver
 * @return true if all fragments of packet were sent
 */
bool Handler::send_complete_message(UdpServer_linux &server, unsigned char *const raw_content_buf, size_t len,
                                    unsigned int requestID, const sockaddr_storage &receiver) {
    BytePtr result;
    BytePtr partial_header;
    unsigned int partial_header_len = utils::pack(partial_header, requestID, (int) len);

    int status;
    bool successful_sent = true;

    size_t cur_content_len = MAX_CONTENT_SIZE;
    unsigned int fragments_to_send = (unsigned int) len / MAX_CONTENT_SIZE + 1;
    for (unsigned int cur_frag_no = 0; cur_frag_no < fragments_to_send; ++cur_frag_no) {
        if (cur_frag_no == fragments_to_send - 1) cur_content_len = len % MAX_CONTENT_SIZE;

        utils::pack(result, partial_header_len, partial_header.get(), cur_frag_no,
                    (unsigned int) cur_content_len, raw_content_buf + cur_frag_no * MAX_CONTENT_SIZE);
        status = server.send_packet(result.get(), cur_content_len + HEADER_SIZE, receiver);
        if (status == UdpServer_linux::SIMULATED_FAILURE) {
            spdlog::warn("Simulated failure: Did not send message #{} fragment {} to {}",
                         requestID, cur_frag_no, utils::get_in_addr_port_str(receiver));
            successful_sent = false;
        }
    }
    return successful_sent;
}

MessagePair Handler::retrieve_stored_message(const sockaddr_storage &client_address, unsigned int requestID) {
    return stored_messages[client_address][requestID];
}

/**Checks if message is ack or duplicate, if so it handles this case and returns true
 *
 * @param server
 * @param buffer
 * @param client_address
 * @param requestID
 * @param fragment_no
 * @return
 */
bool
Handler::handle_ack_or_duplicate(UdpServer_linux &server, unsigned char *buffer, const sockaddr_storage &client_address,
                                 unsigned int requestID, unsigned int fragment_no) {
    if (is_ACK(buffer + HEADER_SIZE)) {
        spdlog::debug("Received ack from {} for requestID {}", utils::get_in_addr_port_str(client_address),
                      requestID);
        handle_ACK(requestID, client_address);
        return true;
    }
    if (is_duplicate_request(&client_address, requestID) && 0 == fragment_no) {
        MessagePair msg_pair = retrieve_stored_message(client_address, requestID);
        bool success_sent = send_complete_message(server, msg_pair.first.get(), msg_pair.second, requestID,
                                                  client_address);
        if (success_sent)
            spdlog::info("Duplicate request, resend message {} to {}:{}", requestID,
                         utils::get_in_addr_str(&client_address), utils::get_in_port(&client_address));
        else
            spdlog::warn("Simulated Failure: Duplicate request, did not successfully resend message {} to {}:{}",
                         requestID,
                         utils::get_in_addr_str(&client_address), utils::get_in_port(&client_address));
        return true;
    }
    return false;
}

/**Checks if the specific expected packet is being received
 *
 * If from different sender, tell him that server is busy
 *
 * @param server
 * @param exp_address
 * @param client_address
 * @param exp_requestID
 * @param requestID
 * @param exp_fragment_no
 * @param fragment_no
 * @param dest_buf
 * @param recv_buffer
 * @param n
 * @return
 */
bool Handler::check_if_correct_packet(UdpServer_linux &server, const sockaddr_storage &exp_address,
                                      const sockaddr_storage &client_address, unsigned int exp_requestID,
                                      unsigned int requestID, unsigned int exp_fragment_no, unsigned int fragment_no) {
    if (utils::is_similar_sockaddr_storage(exp_address, client_address)) { //correct sender
        return exp_requestID == requestID &&
               exp_fragment_no == fragment_no;         //ignore wrong packet from correct sender
    } else { // wrong sender, tell sender that server is busy
        BytePtr busy_server_msg;
        unsigned int busy_server_msg_len = utils::pack(busy_server_msg, constants::SERVER_BUSY);
        bool successful = send_complete_message(server, busy_server_msg.get(), busy_server_msg_len, requestID,
                                                client_address);
        if (successful)
            spdlog::info("Sent 'Server busy' as reply to #{} of {}", requestID,
                         utils::get_in_addr_port_str(client_address));
        else
            spdlog::warn("Simulated Failure: 'Server busy' was not successfully sent as a reply to #{} of {}",
                         requestID,
                         utils::get_in_addr_port_str(client_address));
        return false;
    }
}

/**Receive any or a specific packet (Client, port, requestID, fragment_no)
 *
 * Handles incoming ACK's and duplicate requests
 *
 * @param server
 * @param semantic
 * @param rcv_specifier specifies whether we want a timeout and whether we expect a specific packet
 *     if we do not expect a specific packet, rcv_specifier.expAddress will be set as the receiver
 * @param dest_buf
 * @return status code
 */
int Handler::receive_specific_packet(UdpServer_linux &server, int semantic, ReceiveSpecifier &rcv_specifier,
                                     unsigned char *dest_buf) {
    while (!rcv_specifier.is_timeouted()) {
        int s, usec;
        rcv_specifier.time_in_s_usec(s, usec);

        unsigned char recv_buffer[constants::MAX_PACKET_SIZE];
        int n = server.receive_msg(recv_buffer, s, usec);
        if (n >= HEADER_SIZE) {
            unsigned int requestID, overall_size, fragment_no;
            unpack_header(recv_buffer, requestID, overall_size, fragment_no);
            const sockaddr_storage client_address = server.get_client_address();

            if (!rcv_specifier.is_specific()) rcv_specifier.setExpAddress(client_address);

            if (semantic == constants::ATMOST) {
                bool handled = handle_ack_or_duplicate(server, recv_buffer, client_address, requestID, fragment_no);
                if (handled) continue;
            }

            if (rcv_specifier.is_specific()) {
                bool correct_packet = check_if_correct_packet(server, rcv_specifier.getExpAddress(), client_address,
                                                              rcv_specifier.getExpRequestId(), requestID,
                                                              rcv_specifier.getExpFragmentNo(), fragment_no);
                if (!correct_packet) continue;
            }
            memcpy(dest_buf, recv_buffer, n);
            return n;
        } else if (0 < n && n < HEADER_SIZE) {
            spdlog::warn("Received message with size {} < HEADER_SIZE", n);
        } else if (UdpServer_linux::TIMEOUT == n) {
            return DID_NOT_RECEIVE;
        }
    }
    return DID_NOT_RECEIVE;
}

bool Handler::is_ACK(unsigned char *raw_content) {
    int serv_no;
    utils::unpack(raw_content, serv_no);
    constants::Service_type s_type = constants::service_codes.at(serv_no);
    return constants::Service_type::ack_recvd_reply == s_type;
}

void Handler::handle_ACK(unsigned int requestID, const sockaddr_storage &client) {
    auto client_map_it = stored_messages.find(client);
    if (client_map_it != stored_messages.end()) {
        auto &client_map = client_map_it->second;
        auto stored_entry = client_map.find(requestID);
        if (stored_entry != client_map.end()) {
            client_map.erase(stored_entry);
            spdlog::info("Stored message #{} of {} was removed from stored list.",
                         requestID, utils::get_in_addr_port_str(client));
            return;
        }
    }
    spdlog::info("Received ack for #{} of {} but corresponding reply is not in stored list (anymore).",
                 requestID, utils::get_in_addr_port_str(client));
}

void Handler::unpack_header(unsigned char *buf, unsigned int &requestID, unsigned int &overall_size,
                            unsigned int &fragment_no) {
    utils::unpack(buf, requestID, overall_size, fragment_no);
}

Handler::~Handler() {

}


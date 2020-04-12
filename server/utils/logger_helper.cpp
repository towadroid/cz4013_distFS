//
// Created by towa-ubuntu on 12.04.20.
//

#include "logger_helper.hpp"
#include "packing.hpp"
#include "spdlog/spdlog.h"

void utils::print_request(unsigned char *raw) {
    unsigned int service_no;
    unpack(raw, service_no);
    raw += 4;
    using constants::Service_type;
    Service_type service_type = constants::service_codes.at((int) service_no);
    const std::string &service_name = constants::service_names.at(service_type);
    std::string path;
    switch (service_type) {
        case Service_type::read: {
            int offset, count;
            unpack(raw, path, offset, count);
            spdlog::info("\nService: {} ({})\n"
                         "Path: {}\n"
                         "Offset: {}\n"
                         "Count: {}\n", service_no, service_name, path, offset, count);
            break;
        }
        case Service_type::register_client: {
            int interval;
            unpack(raw, path, interval);
            spdlog::info("\nService: {} ({})\n"
                         "Path: {}\n"
                         "Monitor interval: {} (ms)\n",
                         service_no, service_name, path, interval);
            break;
        }
        case Service_type::insert: {
            std::string content;
            int offset;
            unpack(raw, path, offset, content);
            spdlog::info("\nService: {} ({})\n"
                         "Path: {}\n"
                         "Offset: {}\n"
                         "Content:\n{}\n", service_no, service_name, path, offset, content);
            break;
        }
        case Service_type::remove_content:
        case Service_type::remove_last_char:
        case Service_type::file_mod_time:
        case Service_type::create_file:
        case Service_type::remove_file:
        case Service_type::list_dir: {
            unpack(raw, path);
            spdlog::info("\nService: {} ({})\n"
                         "Path: {}\n", service_no, service_name, path);
            break;
        }
        case Service_type::ack_recvd_reply: {
            spdlog::warn(
                    "Tried to print ack_recvd_reply service, this should not happen!");
            break;
        }
        default: {
            spdlog::error("Service could not be identified!");
        }
    }
}

void utils::print_reply(unsigned int service_no, unsigned char *raw, unsigned int length) {
    using constants::Service_type;
    Service_type service_type = constants::service_codes.at((int) service_no);
    const std::string &service_name = constants::service_names.at(service_type);
    int status_no;
    unpack(raw, status_no);
    raw += 4;
    const std::string &status_name = constants::status_constants.at(status_no);
    if (length == 4) {
        spdlog::info("Status no: {} ({})", status_no, status_name);
        return;
    }
    switch (service_type) {
        case Service_type::read: {
            std::string content;
            unpack(raw, content);
            spdlog::info("\nStatus: {} ({})\n"
                         "Content:\n{}", status_no, status_name, content);
            break;
        }
        case Service_type::file_mod_time: {
            int edit_time;
            unpack(raw, edit_time);
            spdlog::info("\nStatus: {} ({}]\n"
                         "Seconds since Unix time: {}", status_no, status_name, edit_time);
            break;
        }
        case Service_type::ack_recvd_reply: {
            spdlog::warn(
                    "Tried to print ack_recvd_reply service, this should not happen!");
            break;
        }
        case Service_type::list_dir: {
            int count;
            unpack(raw, count);
            auto entry_types = new int[count];
            auto entry_names = new std::string[count];
            std::string list_dir{};
            std::string type;
            for (int i = 0; i < count; ++i) {
                type = entry_types[i] ? "file" : "dir";
                list_dir += std::string("\nType: " + type + "\tName: " + entry_names[i]);
            }
            spdlog::info("\nStatus: {} ({}]\n"
                         "Count: {}{}", status_no, status_name, count, list_dir);
            break;
        }
        default: {
            spdlog::error("Service could not be identified!");
        }
    }
}


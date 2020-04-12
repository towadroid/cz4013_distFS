//
// Created by towa-ubuntu on 12.04.20.
//

#ifndef SERVER_LOGGER_HELPER_HPP
#define SERVER_LOGGER_HELPER_HPP

namespace utils {
    //-----------logging -------------------------
    void print_request(unsigned char *raw);

    void print_reply(unsigned int service_no, unsigned char *raw, unsigned int length);
}

#endif //SERVER_LOGGER_HELPER_HPP

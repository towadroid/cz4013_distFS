//
// Created by towa-ubuntu on 05.04.20.
//

#include <arpa/inet.h>
#include "test_resoures.hpp"
#include "../../HelperClasses.hpp"
#include <string>

sockaddr_storage get_client(unsigned int i) {
    sockaddr_storage client{};
    client.ss_family = AF_INET;
    ((sockaddr_in *) &client)->sin_port = htons((short unsigned int) i);
    std::string i_str = std::to_string(i);
    std::string address_str{i_str + "." + i_str + "." + i_str + "." + i_str};
    inet_pton(AF_INET, address_str.c_str(), &((sockaddr_in *) &client)->sin_addr);
    return client;
}

namespace testing::internal {
    bool operator==(const sockaddr_storage &lhs, const sockaddr_storage &rhs) {
        SockaddrStor_Equal sockaddr_equal;
        return sockaddr_equal(lhs, rhs);
    }
}

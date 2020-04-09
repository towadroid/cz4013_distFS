//
// Created by towa-ubuntu on 05.04.20.
//

#include <arpa/inet.h>
#include "test_resoures.hpp"
#include "../HelperClasses/HelperClasses.hpp"
#include <string>

sockaddr_storage test_rsc::get_client(unsigned int i) {
    sockaddr_storage client{};
    client.ss_family = AF_INET;
    ((sockaddr_in *) &client)->sin_port = htons((short unsigned int) i);
    std::string i_str = std::to_string(i);
    std::string address_str{i_str + "." + i_str + "." + i_str + "." + i_str};
    inet_pton(AF_INET, address_str.c_str(), &((sockaddr_in *) &client)->sin_addr);
    return client;
}

void test_rsc::prepare_file(const std::string &file_path, const std::string &to_insert) {
    path test_file_path{constants::FILE_DIR_PATH + file_path};
    try {
        utils::create_file(test_file_path);
    } catch (const File_already_exists &err) {}
    try {
        utils::remove_content_from_file(test_file_path);
    } catch (const File_already_empty &err) {}

    utils::insert_to_file(test_file_path, to_insert,
                          0);
}

namespace testing::internal {
    bool operator==(const sockaddr_storage &lhs, const sockaddr_storage &rhs) {
        SockaddrStor_Equal sockaddr_equal;
        return sockaddr_equal(lhs, rhs);
    }
}

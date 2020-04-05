//
// Created by towa-ubuntu on 05.04.20.
//

#ifndef SERVER_TEST_RESOURES_HPP
#define SERVER_TEST_RESOURES_HPP

#include "gtest/gtest.h"
#include <string>

namespace test_rsc {
    sockaddr_storage get_client(unsigned int i);

    void prepare_file();

    const std::string test_file_content{
            "0123456789\n"
            "123456789\n"
            "123456789\n"
            "123456789\n"
            "123456789\n"
            "123456789\n"
            "123456789\n"
            "123456789\n"
            "123456789\n"
            "123456789\n"
            "123456789\n"
            "123456789\n"
            "123456789\n"
            "123456789\n"
            "123456789\n"
            "123456789\n"
            "123456789\n"
            "123456789\n"
            "123456789\n"
            "123456789\n"
            "123456789\n"
    };
}

namespace testing::internal {
    bool operator==(const sockaddr_storage &lhs, const sockaddr_storage &rhs);
}

#endif //SERVER_TEST_RESOURES_HPP

//
// Created by towa-ubuntu on 05.04.20.
//

#ifndef SERVER_TEST_RESOURES_HPP
#define SERVER_TEST_RESOURES_HPP

#include "gtest/gtest.h"


sockaddr_storage get_client(unsigned int i);

namespace testing::internal {
    bool operator==(const sockaddr_storage &lhs, const sockaddr_storage &rhs);
}

#endif //SERVER_TEST_RESOURES_HPP

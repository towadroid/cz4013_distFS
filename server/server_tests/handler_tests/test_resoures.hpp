//
// Created by towa-ubuntu on 05.04.20.
//

#ifndef SERVER_TEST_RESOURES_HPP
#define SERVER_TEST_RESOURES_HPP

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <string>
#include "../mock_classes/MockHandler.hpp"
#include "../mock_classes/MockUdpServer_linux.hpp"
#include "../../utils/packing.hpp"

namespace test_rsc {
    sockaddr_storage get_client(unsigned int i);

    void prepare_file(const std::string &file_path = std::string{"test_file"});

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


using testing::SetArrayArgument;
using testing::ReturnRef;
using testing::DoAll;
using testing::Return;
using testing::Args;
using testing::ElementsAreArray;
using testing::_;
using testing::WithArg;

using test_rsc::prepare_file;
using test_rsc::get_client;

#endif //SERVER_TEST_RESOURES_HPP

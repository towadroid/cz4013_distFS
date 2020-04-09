//
// Created by towa-ubuntu on 3/18/20.
//
#include <arpa/inet.h>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "../mock_classes/MockHandler.hpp"
#include "../mock_classes/MockUdpServer_linux.hpp"
#include "../../constants.hpp"
#include "../../UdpServer_linux.hpp"
#include "../../utils/packing.hpp"
#include "spdlog/spdlog.h"
#include "../test_resoures.hpp"

TEST(Handler, service_insert) {
    Handler h{};
    Service_type serviceType = constants::service_codes.at(2);
    UdpServer_linux server(2302);
    BytePtr raw_content;
    std::string f_name{"test_ins"}; //12
    std::string to_write{"GERM"}; //8
    utils::pack(raw_content, 2, f_name, 3, to_write);
    BytePtr raw_reply;
    unsigned int raw_reply_length;
    h.service(serviceType, server, raw_content.get(), raw_reply, raw_reply_length, get_client(1), 0);

}

TEST(Handler, pack) {
    BytePtr result, raw_reply_content;
    BytePtr partial_header;
    unsigned int partial_header_len = utils::pack(partial_header, 0, (int) 4);
    unsigned int raw_reply_len = utils::pack(raw_reply_content, constants::SUCCESS);
    unsigned char *raw_content_buf = raw_reply_content.get();

    utils::pack(result, partial_header_len, partial_header.get(), 0, raw_reply_len, raw_content_buf);
}

TEST(Handler, service_read) {
    Handler h{};
    Service_type serviceType = constants::service_codes.at(1);
    UdpServer_linux server(2302);
    BytePtr raw_content;
    BytePtr raw_reply;
    unsigned int raw_reply_length;
    std::string filename{"file1"};
    utils::pack(raw_content, 2, filename, 3, 5);
    h.service(serviceType, server, raw_content.get(), raw_reply, raw_reply_length, get_client(1), 0);
}

using testing::SetArrayArgument;
using testing::ReturnRef;
using testing::DoAll;
using testing::Return;
using testing::_;
using testing::ElementsAreArray;
using testing::Args;

using test_rsc::get_client;

TEST(Handler, read) {
    MockHandler mock_handler;
    MockUdpServer_linux mock_server;
    prepare_file();

    sockaddr_storage client1 = get_client(1);

    BytePtr raw1;
    unsigned int raw1_len = utils::pack(raw1, 1, std::string("test_file"), 0, 5);

    BytePtr inc_msg1;
    unsigned int inc1_len = utils::pack(inc_msg1, 0, raw1_len, 0, raw1_len, raw1.get());

    EXPECT_CALL(mock_server, receive_msg_impl)
            .WillOnce(DoAll(
                    SetArrayArgument<0>(inc_msg1.get(), inc_msg1.get() + inc1_len),
                    Return(inc1_len)));
    EXPECT_CALL(mock_server, get_client_address).WillRepeatedly(ReturnRef(client1));
    EXPECT_CALL(mock_handler, send_complete_message);
    mock_handler.receive_handle_message(mock_server, constants::ATLEAST);
}

TEST(Match, easy_expect) {
    MockUdpServer_linux mock_server;
    EXPECT_CALL(mock_server, receive_msg_impl(nullptr, 0, 0))
            .WillOnce(Return(15));
    mock_server.receive_msg_impl(nullptr, 0, 0);
}

TEST(Match, array) {
    MockUdpServer_linux mock_server;
    MockHandler mock_handler;
    unsigned char arr[] = {0x12, 0x34, 0x56};
    EXPECT_CALL(mock_handler, send_complete_message(_, _, _, _, _))
            .With(Args<1, 2>(ElementsAreArray(arr)));
    mock_handler.send_complete_message(mock_server, arr, 3, 0, get_client(1));
}

// Show how to return by reference work and how to set an array
TEST(Action, ref) {
    MockUdpServer_linux mock_server;
    sockaddr_storage client1 = get_client(1);
    EXPECT_CALL(mock_server, get_client_address).WillOnce(ReturnRef(client1));

    sockaddr_storage tmp = mock_server.get_client_address();
    std::cout << utils::get_in_addr_port_str(tmp) << std::endl;
}

//sho how to mock side effects, here: set the array parameter which is intended as output
TEST(Action, array_side_effect) {
    MockUdpServer_linux mock_server;

    BytePtr incoming_msg1;
    unsigned int inc1_len = utils::pack(incoming_msg1, std::string("abcdef"));

    EXPECT_CALL(mock_server, receive_msg_impl)
            .WillOnce(DoAll(
                    SetArrayArgument<0>(incoming_msg1.get(), incoming_msg1.get() + inc1_len),
                    Return(inc1_len)));

    unsigned char a[256];
    mock_server.receive_msg(a);
    std::string res;
    utils::unpack(a, res);
    std::cout << res << std::endl;
}

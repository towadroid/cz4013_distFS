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
    h.service(serviceType, server, raw_content.get(), raw_reply, raw_reply_length);

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
    h.service(serviceType, server, raw_content.get(), raw_reply, raw_reply_length);
}

sockaddr_storage get_client1() {
    sockaddr_storage client{};
    client.ss_family = AF_INET;
    ((sockaddr_in *) &client)->sin_port = htons(1234);
    inet_pton(AF_INET, "123.0.0.1", &((sockaddr_in *) &client)->sin_addr);
    return client;
}

sockaddr_storage get_client2() {
    sockaddr_storage client{};
    client.ss_family = AF_INET;
    ((sockaddr_in *) &client)->sin_port = htons(1234);
    inet_pton(AF_INET, "123.0.0.2", &((sockaddr_in *) &client)->sin_addr);
    return client;
}

using testing::SetArrayArgument;
using testing::ReturnRef;
using testing::DoAll;
using testing::Return;

TEST(Handler, read) {
    spdlog::set_level(spdlog::level::trace);
    Handler h{};
    MockUdpServer_linux mock_server;

    sockaddr_storage client1 = get_client1();

    BytePtr raw1;
    unsigned int raw1_len = utils::pack(raw1, 1, std::string("file1"), 0, 5);

    BytePtr inc_msg1;
    unsigned int inc1_len = utils::pack(inc_msg1, 0, raw1_len, 0, raw1_len, raw1.get());

    EXPECT_CALL(mock_server, receive_msg_impl)
            .WillOnce(DoAll(
                    SetArrayArgument<0>(inc_msg1.get(), inc_msg1.get() + inc1_len),
                    Return(inc1_len)
                      )
            );
    EXPECT_CALL(mock_server, get_client_address).WillRepeatedly(ReturnRef(client1));
    h.receive_handle_message(mock_server, constants::ATLEAST);
}

// need to make it multiple packets
TEST(Handler, multiple_packets) {
    Handler h{};
    MockUdpServer_linux mock_server;

    sockaddr_storage client1 = get_client1();

    /* assume
     * constexpr int HEADER_SIZE = 12;
     * // this is the overall max packet size, including header
     * constexpr int MAX_PACKET_SIZE = 256;*/
    std::string long_path{};
    for (int i = 5; i <= 244; ++i)
        long_path.append("o");

    std::string long_path2{};
    for (int i = 249; i <= 300; ++i)
        long_path2.append("o");
    //4 more byte added per string when packed

    BytePtr inc_msg1;
    unsigned int inc1_len1 = utils::pack(inc_msg1, 0, 300, 0, long_path);

    BytePtr inc_msg2;
    unsigned int inc1_len2 = utils::pack(inc_msg2, 0, 300, 1, long_path2);

    EXPECT_CALL(mock_server, receive_msg_impl)
            .Times(2)
            .WillOnce(DoAll(
                    SetArrayArgument<0>(inc_msg1.get(), inc_msg1.get() + inc1_len1),
                    Return(inc1_len1)))
            .WillOnce(DoAll(
                    SetArrayArgument<0>(inc_msg2.get(), inc_msg2.get() + inc1_len2),
                    Return(inc1_len2)));
    EXPECT_CALL(mock_server, get_client_address).WillRepeatedly(ReturnRef(client1));
    h.receive_handle_message(mock_server, constants::ATLEAST);
}

// Show how to return by reference work and how to set an array
TEST(Action, ref) {
    MockUdpServer_linux mock_server;
    sockaddr_storage client1 = get_client1();
    EXPECT_CALL(mock_server, get_client_address).WillOnce(ReturnRef(client1));

    sockaddr_storage tmp = mock_server.get_client_address();
    std::cout << utils::get_in_addr_port_str(tmp) << std::endl;

    BytePtr incoming_mgs1;
    unsigned int inc1_len = utils::pack(incoming_mgs1, std::string("abcdef"));

    EXPECT_CALL(mock_server, receive_msg_impl)
            .WillOnce(SetArrayArgument<0>(incoming_mgs1.get(), incoming_mgs1.get() + inc1_len)
            );

    unsigned char a[256];
    mock_server.receive_msg(a);
    std::string res;
    utils::unpack(a, res);
    std::cout << res << std::endl;
}

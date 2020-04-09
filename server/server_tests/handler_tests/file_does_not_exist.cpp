//
// Created by towa-ubuntu on 07.04.20.
//

/**This file tests all services offered to the client, with file does not exist
 * All tests follow the scenario that
 *      request and reply fit in one packet
 */

#include "../test_resoures.hpp"

TEST(Handler_file_does_not_exist, read) {
    MockHandler mock_handler;
    MockUdpServer_linux mock_server;

    sockaddr_storage client1 = get_client(1);
    unsigned int requestID = 0;

    BytePtr raw1;
    unsigned int raw1_len = utils::pack(raw1, 1, std::string("non_existing_file"), 0, 5);

    BytePtr inc_msg1;
    unsigned int inc1_len = utils::pack(inc_msg1, requestID, raw1_len, 0, raw1_len, raw1.get());

    EXPECT_CALL(mock_server, receive_msg_impl)
            .WillOnce(DoAll(
                    SetArrayArgument<0>(inc_msg1.get(), inc_msg1.get() + inc1_len),
                    Return(inc1_len)
                      )
            );
    EXPECT_CALL(mock_server, get_client_address).WillOnce(ReturnRef(client1));

    BytePtr er_raw; //expected reply raw
    unsigned int er_raw_len = utils::pack(er_raw, constants::FILE_DOES_NOT_EXIST);

    EXPECT_CALL(mock_handler, send_complete_message(_, _, _, requestID, client1))
            .With(Args<1, 2>(ElementsAreArray(er_raw.get(), er_raw_len)));

    mock_handler.receive_handle_message(mock_server, constants::ATLEAST);
}

TEST(Handler_file_does_not_exist, insert) {
    MockHandler mock_handler;
    MockUdpServer_linux mock_server;

    sockaddr_storage client1 = get_client(1);
    unsigned int requestID = 0;
    std::string to_insert{"I want to insert this!"};

    BytePtr raw1;
    unsigned int raw1_len = utils::pack(raw1, 2, std::string("non_existing_file"), 0, to_insert);

    BytePtr inc_msg1;
    unsigned int inc1_len = utils::pack(inc_msg1, requestID, raw1_len, 0, raw1_len, raw1.get());

    EXPECT_CALL(mock_server, receive_msg_impl)
            .WillOnce(DoAll(
                    SetArrayArgument<0>(inc_msg1.get(), inc_msg1.get() + inc1_len),
                    Return(inc1_len)));
    EXPECT_CALL(mock_server, get_client_address).WillOnce(ReturnRef(client1));

    BytePtr er_raw; //expected reply raw
    unsigned int er_raw_len = utils::pack(er_raw, constants::FILE_DOES_NOT_EXIST);

    EXPECT_CALL(mock_handler, send_complete_message(_, _, _, requestID, client1))
            .With(Args<1, 2>(ElementsAreArray(er_raw.get(), er_raw_len)));

    mock_handler.receive_handle_message(mock_server, constants::ATLEAST);
}

TEST(Handler_file_does_not_exist, remove_content) {
    MockHandler mock_handler;
    MockUdpServer_linux mock_server;

    sockaddr_storage client1 = get_client(1);
    unsigned int requestID = 0;

    BytePtr raw1;
    unsigned int raw1_len = utils::pack(raw1, 4, std::string("non_existing_file"));

    BytePtr inc_msg1;
    unsigned int inc1_len = utils::pack(inc_msg1, requestID, raw1_len, 0, raw1_len, raw1.get());

    EXPECT_CALL(mock_server, receive_msg_impl)
            .WillOnce(DoAll(
                    SetArrayArgument<0>(inc_msg1.get(), inc_msg1.get() + inc1_len),
                    Return(inc1_len)));
    EXPECT_CALL(mock_server, get_client_address).WillOnce(ReturnRef(client1));

    BytePtr er_raw; //expected reply raw
    unsigned int er_raw_len = utils::pack(er_raw, constants::FILE_DOES_NOT_EXIST);

    EXPECT_CALL(mock_handler, send_complete_message(_, _, _, requestID, client1))
            .With(Args<1, 2>(ElementsAreArray(er_raw.get(), er_raw_len)));

    mock_handler.receive_handle_message(mock_server, constants::ATLEAST);
}

TEST(Handler_file_does_not_exist, remove_last_char) {
    MockHandler mock_handler;
    MockUdpServer_linux mock_server;

    sockaddr_storage client1 = get_client(1);
    unsigned int requestID = 0;

    BytePtr raw1;
    unsigned int raw1_len = utils::pack(raw1, 5, std::string("non_existing_file"));

    BytePtr inc_msg1;
    unsigned int inc1_len = utils::pack(inc_msg1, requestID, raw1_len, 0, raw1_len, raw1.get());

    EXPECT_CALL(mock_server, receive_msg_impl)
            .WillOnce(DoAll(
                    SetArrayArgument<0>(inc_msg1.get(), inc_msg1.get() + inc1_len),
                    Return(inc1_len)));
    EXPECT_CALL(mock_server, get_client_address).WillOnce(ReturnRef(client1));

    BytePtr er_raw; //expected reply raw
    unsigned int er_raw_len = utils::pack(er_raw, constants::FILE_DOES_NOT_EXIST);

    EXPECT_CALL(mock_handler, send_complete_message(_, _, _, requestID, client1))
            .With(Args<1, 2>(ElementsAreArray(er_raw.get(), er_raw_len)));

    mock_handler.receive_handle_message(mock_server, constants::ATLEAST);
}

TEST(Handler_file_does_not_exist, last_mod_time) {
    MockHandler mock_handler;
    MockUdpServer_linux mock_server;

    sockaddr_storage client1 = get_client(1);
    unsigned int requestID = 0;

    BytePtr raw1;
    unsigned int raw1_len = utils::pack(raw1, 6, std::string("non_existing_file"));

    BytePtr inc_msg1;
    unsigned int inc1_len = utils::pack(inc_msg1, requestID, raw1_len, 0, raw1_len, raw1.get());

    EXPECT_CALL(mock_server, receive_msg_impl)
            .WillOnce(DoAll(
                    SetArrayArgument<0>(inc_msg1.get(), inc_msg1.get() + inc1_len),
                    Return(inc1_len)));
    EXPECT_CALL(mock_server, get_client_address).WillOnce(ReturnRef(client1));

    BytePtr er_raw; //expected reply raw
    unsigned int er_raw_len = utils::pack(er_raw, constants::FILE_DOES_NOT_EXIST);

    EXPECT_CALL(mock_handler, send_complete_message(_, _, _, requestID, client1))
            .With(Args<1, 2>(ElementsAreArray(er_raw.get(), er_raw_len)));

    mock_handler.receive_handle_message(mock_server, constants::ATLEAST);
}

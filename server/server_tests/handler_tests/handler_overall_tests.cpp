//
// Created by towa-ubuntu on 05.04.20.
//

/**This file tests all services offered to the client, except for 'Monitor' and 'Ack rcvd reply'
 * All tests follow the scenario that
 *      request and reply fit in one packet
 *      all operations can be executed successfully (always status=SUCCESS)
 */

#include "../test_resoures.hpp"


TEST(Handler_overall, read) {
    MockHandler mock_handler;
    MockUdpServer_linux mock_server;
    std::string file_content{"01234567890123456789012345678901234567890123456789\n"
                             "\n"
                             "This is read only content!\n"};
    prepare_file("test_file", file_content);

    sockaddr_storage client1 = get_client(1);
    unsigned int requestID = 0;

    BytePtr raw1;
    unsigned int raw1_len = utils::pack(raw1, 1, std::string("test_file"), 0, 5);

    BytePtr inc_msg1;
    unsigned int inc1_len = utils::pack(inc_msg1, requestID, raw1_len, 0, raw1_len, raw1.get());

    EXPECT_CALL(mock_server, receive_msg_impl)
            .WillOnce(DoAll(
                    SetArrayArgument<0>(inc_msg1.get(), inc_msg1.get() + inc1_len),
                    Return(inc1_len)));
    EXPECT_CALL(mock_server, get_client_address).WillOnce(ReturnRef(client1));


    BytePtr er_raw; //expected reply raw
    unsigned int er_raw_len = utils::pack(er_raw, constants::SUCCESS, file_content);

    EXPECT_CALL(mock_handler, send_complete_message(_, _, _, requestID, client1))
            .With(Args<1, 2>(ElementsAreArray(er_raw.get(), er_raw_len)));

    mock_handler.receive_handle_message(mock_server, constants::ATLEAST);
}

TEST(Handler_overall, insert) {
    MockHandler mock_handler;
    MockUdpServer_linux mock_server;
    prepare_file();

    sockaddr_storage client1 = get_client(1);
    unsigned int requestID = 0;
    std::string to_insert{"I want to insert this!"};

    BytePtr raw1;
    unsigned int raw1_len = utils::pack(raw1, 2, std::string("test_file"), 0, to_insert);

    BytePtr inc_msg1;
    unsigned int inc1_len = utils::pack(inc_msg1, requestID, raw1_len, 0, raw1_len, raw1.get());

    EXPECT_CALL(mock_server, receive_msg_impl)
            .WillOnce(DoAll(
                    SetArrayArgument<0>(inc_msg1.get(), inc_msg1.get() + inc1_len),
                    Return(inc1_len)));
    EXPECT_CALL(mock_server, get_client_address).WillOnce(ReturnRef(client1));

    BytePtr er_raw; //expected reply raw
    unsigned int er_raw_len = utils::pack(er_raw, constants::SUCCESS);

    EXPECT_CALL(mock_handler, send_complete_message(_, _, _, requestID, client1))
            .With(Args<1, 2>(ElementsAreArray(er_raw.get(), er_raw_len)));

    mock_handler.receive_handle_message(mock_server, constants::ATLEAST);

    std::string test_file_actual_content = utils::read_file_to_string(constants::FILE_DIR_PATH + "test_file");
    EXPECT_EQ(test_file_actual_content, to_insert.append(test_rsc::test_file_content));
}

TEST(Handler_overall, remove_content) {
    MockHandler mock_handler;
    MockUdpServer_linux mock_server;
    prepare_file();

    sockaddr_storage client1 = get_client(1);
    unsigned int requestID = 0;

    BytePtr raw1;
    unsigned int raw1_len = utils::pack(raw1, 4, std::string("test_file"));

    BytePtr inc_msg1;
    unsigned int inc1_len = utils::pack(inc_msg1, requestID, raw1_len, 0, raw1_len, raw1.get());

    EXPECT_CALL(mock_server, receive_msg_impl)
            .WillOnce(DoAll(
                    SetArrayArgument<0>(inc_msg1.get(), inc_msg1.get() + inc1_len),
                    Return(inc1_len)));
    EXPECT_CALL(mock_server, get_client_address).WillOnce(ReturnRef(client1));

    BytePtr er_raw; //expected reply raw
    unsigned int er_raw_len = utils::pack(er_raw, constants::SUCCESS);

    EXPECT_CALL(mock_handler, send_complete_message(_, _, _, requestID, client1))
            .With(Args<1, 2>(ElementsAreArray(er_raw.get(), er_raw_len)));

    mock_handler.receive_handle_message(mock_server, constants::ATLEAST);

    std::string test_file_actual_content = utils::read_file_to_string(constants::FILE_DIR_PATH + "test_file");
    EXPECT_EQ(test_file_actual_content, std::string{""});
}

TEST(Handler_overall, remove_last_char) {
    MockHandler mock_handler;
    MockUdpServer_linux mock_server;
    prepare_file();

    sockaddr_storage client1 = get_client(1);
    unsigned int requestID = 0;

    BytePtr raw1;
    unsigned int raw1_len = utils::pack(raw1, 5, std::string("test_file"));

    BytePtr inc_msg1;
    unsigned int inc1_len = utils::pack(inc_msg1, requestID, raw1_len, 0, raw1_len, raw1.get());

    EXPECT_CALL(mock_server, receive_msg_impl)
            .WillOnce(DoAll(
                    SetArrayArgument<0>(inc_msg1.get(), inc_msg1.get() + inc1_len),
                    Return(inc1_len)));
    EXPECT_CALL(mock_server, get_client_address).WillOnce(ReturnRef(client1));

    BytePtr er_raw; //expected reply raw
    unsigned int er_raw_len = utils::pack(er_raw, constants::SUCCESS);

    EXPECT_CALL(mock_handler, send_complete_message(_, _, _, requestID, client1))
            .With(Args<1, 2>(ElementsAreArray(er_raw.get(), er_raw_len)));

    mock_handler.receive_handle_message(mock_server, constants::ATLEAST);

    std::string test_file_actual_content = utils::read_file_to_string(constants::FILE_DIR_PATH + "test_file");
    std::string expected_file_content = test_rsc::test_file_content;
    expected_file_content.pop_back();
    EXPECT_EQ(test_file_actual_content, expected_file_content);
}

TEST(Handler_overall, last_mod_time) {
    MockHandler mock_handler;
    MockUdpServer_linux mock_server;
    prepare_file();

    sockaddr_storage client1 = get_client(1);
    unsigned int requestID = 0;

    BytePtr raw1;
    unsigned int raw1_len = utils::pack(raw1, 6, std::string("test_file"));

    BytePtr inc_msg1;
    unsigned int inc1_len = utils::pack(inc_msg1, requestID, raw1_len, 0, raw1_len, raw1.get());

    EXPECT_CALL(mock_server, receive_msg_impl)
            .WillOnce(DoAll(
                    SetArrayArgument<0>(inc_msg1.get(), inc_msg1.get() + inc1_len),
                    Return(inc1_len)));
    EXPECT_CALL(mock_server, get_client_address).WillOnce(ReturnRef(client1));

    int actual_last_mod_time = utils::get_last_mod_time(path{constants::FILE_DIR_PATH + "test_file"});

    BytePtr er_raw; //expected reply raw
    unsigned int er_raw_len = utils::pack(er_raw, constants::SUCCESS, actual_last_mod_time);

    EXPECT_CALL(mock_handler, send_complete_message(_, _, _, requestID, client1))
            .With(Args<1, 2>(ElementsAreArray(er_raw.get(), er_raw_len)));

    mock_handler.receive_handle_message(mock_server, constants::ATLEAST);
}

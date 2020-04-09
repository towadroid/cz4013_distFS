//
// Created by towa-ubuntu on 07.04.20.
//

/**This file tests all services offered to the client, with other failures
 * + special case remove  content for empty file
 * All tests follow the scenario that
 *      request and reply fit in one packet
 */

#include "../test_resoures.hpp"

TEST(failing_request, read_offset_count_exceed) {
    MockHandler mock_handler;
    MockUdpServer_linux mock_server;
    prepare_file();

    sockaddr_storage client1 = get_client(1);
    unsigned int requestID = 0;

    BytePtr raw1;
    unsigned int raw1_len = utils::pack(raw1, 1, std::string("test_file"), 0, 10000);

    BytePtr inc_msg1;
    unsigned int inc1_len = utils::pack(inc_msg1, requestID, raw1_len, 0, raw1_len, raw1.get());

    EXPECT_CALL(mock_server, receive_msg_impl)
            .WillOnce(DoAll(
                    SetArrayArgument<0>(inc_msg1.get(), inc_msg1.get() + inc1_len),
                    Return(inc1_len)));
    EXPECT_CALL(mock_server, get_client_address).WillOnce(ReturnRef(client1));

    BytePtr er_raw; //expected reply raw
    unsigned int er_raw_len = utils::pack(er_raw, constants::OFFSET_OUT_OF_BOUNDS);

    EXPECT_CALL(mock_handler, send_complete_message(_, _, _, requestID, client1))
            .With(Args<1, 2>(ElementsAreArray(er_raw.get(), er_raw_len)));

    mock_handler.receive_handle_message(mock_server, constants::ATLEAST);
}

TEST(failing_request, insert_offset_exceed) {
    MockHandler mock_handler;
    MockUdpServer_linux mock_server;
    prepare_file();

    sockaddr_storage client1 = get_client(1);
    unsigned int requestID = 0;
    std::string to_insert{"I want to insert this!"};

    BytePtr raw1;
    unsigned int raw1_len = utils::pack(raw1, 2, std::string("test_file"), 10000, to_insert);

    BytePtr inc_msg1;
    unsigned int inc1_len = utils::pack(inc_msg1, requestID, raw1_len, 0, raw1_len, raw1.get());

    EXPECT_CALL(mock_server, receive_msg_impl)
            .WillOnce(DoAll(
                    SetArrayArgument<0>(inc_msg1.get(), inc_msg1.get() + inc1_len),
                    Return(inc1_len)));
    EXPECT_CALL(mock_server, get_client_address).WillOnce(ReturnRef(client1));

    BytePtr er_raw; //expected reply raw
    unsigned int er_raw_len = utils::pack(er_raw, constants::OFFSET_OUT_OF_BOUNDS);

    EXPECT_CALL(mock_handler, send_complete_message(_, _, _, requestID, client1))
            .With(Args<1, 2>(ElementsAreArray(er_raw.get(), er_raw_len)));

    mock_handler.receive_handle_message(mock_server, constants::ATLEAST);
}

TEST(failing_request, remove_last_char_already_empty) {
    MockHandler mock_handler;
    MockUdpServer_linux mock_server;
    prepare_file();
    try {
        utils::remove_content_from_file(constants::FILE_DIR_PATH + "test_file");
    } catch (const File_already_empty &err) {}

    sockaddr_storage client1 = get_client(1);
    unsigned int requestID = 0;

    BytePtr raw1;
    unsigned int raw1_len = utils::pack(raw1, 5, std::string("test_file"));

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
    unsigned int er_raw_len = utils::pack(er_raw, constants::FILE_ALREADY_EMPTY);

    EXPECT_CALL(mock_handler, send_complete_message(_, _, _, requestID, client1))
            .With(Args<1, 2>(ElementsAreArray(er_raw.get(), er_raw_len)));

    mock_handler.receive_handle_message(mock_server, constants::ATLEAST);
}

TEST(failing_request, remove_content_already_empty) {
    MockHandler mock_handler;
    MockUdpServer_linux mock_server;
    prepare_file();
    try {
        utils::remove_content_from_file(constants::FILE_DIR_PATH + "test_file");
    } catch (const File_already_empty &err) {}

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

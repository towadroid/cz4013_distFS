//
// Created by towa-ubuntu on 07.04.20.
//

/**This file tests fragmented messages
 * All tests follow the scenario that
 *      all operations can be executed successfully (always status=SUCCESS)
 */

#include "../test_resoures.hpp"

TEST(fragments, two_following) {
    MockHandler mock_handler;
    MockUdpServer_linux mock_server;
    prepare_file();

    sockaddr_storage client1 = get_client(1);
    unsigned int requestID = 0;
    std::string to_insert{};

    for (int i = 1; i <= constants::MAX_CONTENT_SIZE + 20; ++i) {
        to_insert.append("o");
        if (i % 30 == 0) to_insert.append("\n");
    }


    unsigned int first_string_len = constants::MAX_CONTENT_SIZE - 4 - 4 - 13 - 4;
    //str_length, offset, file string, service no
    auto overall_string_len = (unsigned int) to_insert.length();
    unsigned int sec_string_len = overall_string_len - first_string_len;

    auto *buffer = new unsigned char[first_string_len];
    memcpy(buffer, to_insert.c_str(), first_string_len);

    BytePtr raw1;
    unsigned int raw1_len = utils::pack(raw1, 2, std::string("test_file"), 0, overall_string_len,
                                        first_string_len, buffer);

    EXPECT_EQ(constants::MAX_CONTENT_SIZE, raw1_len); //first packet should be full

    memcpy(buffer, to_insert.c_str() + first_string_len, sec_string_len);

    BytePtr raw2;
    unsigned int raw2_len = utils::pack(raw2, sec_string_len, buffer);

    BytePtr inc_frag1;
    unsigned int inc1_len = utils::pack(inc_frag1, requestID, raw1_len + raw2_len, 0, raw1_len, raw1.get());

    BytePtr inc_frag2;
    unsigned int inc2_len = utils::pack(inc_frag2, requestID, raw1_len + raw2_len, 1, raw2_len, raw2.get());

    EXPECT_CALL(mock_server, receive_msg_impl)
            .WillOnce(DoAll(
                    SetArrayArgument<0>(inc_frag1.get(), inc_frag1.get() + inc1_len),
                    Return(inc1_len)))
            .WillOnce(DoAll(
                    SetArrayArgument<0>(inc_frag2.get(), inc_frag2.get() + inc2_len),
                    Return(inc2_len)));
    EXPECT_CALL(mock_server, get_client_address)
            .WillOnce(ReturnRef(client1))
            .WillOnce(ReturnRef(client1));

    BytePtr er_raw; //expected reply raw
    unsigned int er_raw_len = utils::pack(er_raw, constants::SUCCESS);

    EXPECT_CALL(mock_handler, send_complete_message(_, _, _, requestID, client1))
            .With(Args<1, 2>(ElementsAreArray(er_raw.get(), er_raw_len)));

    mock_handler.receive_handle_message(mock_server, constants::ATLEAST);

    std::string test_file_actual_content = utils::read_file_to_string(constants::FILE_DIR_PATH + "test_file");
    EXPECT_EQ(test_file_actual_content, to_insert.append(test_rsc::test_file_content));

    delete[] buffer;
}

TEST(fragments, wrong_fragment_correct_sender) {
    MockHandler mock_handler;
    MockUdpServer_linux mock_server;
    prepare_file();

    sockaddr_storage client1 = get_client(1);
    unsigned int requestID = 0;
    std::string to_insert{};

    for (int i = 1; i <= constants::MAX_CONTENT_SIZE + 20; ++i) {
        to_insert.append("o");
        if (i % 30 == 0) to_insert.append("\n");
    }


    unsigned int first_string_len = constants::MAX_CONTENT_SIZE - 4 - 4 - 13 - 4;
    //str_length, offset, file string, service no
    auto overall_string_len = (unsigned int) to_insert.length();
    unsigned int sec_string_len = overall_string_len - first_string_len;

    auto *buffer = new unsigned char[first_string_len];
    memcpy(buffer, to_insert.c_str(), first_string_len);

    BytePtr raw1_1;
    unsigned int raw1_len = utils::pack(raw1_1, 2, std::string("test_file"), 0, overall_string_len,
                                        first_string_len, buffer);

    EXPECT_EQ(constants::MAX_CONTENT_SIZE, raw1_len); //first packet should be full

    memcpy(buffer, to_insert.c_str() + first_string_len, sec_string_len);

    BytePtr raw1_2;
    unsigned int raw1_2_len = utils::pack(raw1_2, sec_string_len, buffer);

    BytePtr raw2;
    unsigned int raw2_len = utils::pack(raw2, 2, std::string("test_file"), 0, std::string{"should not end up in file"});

    BytePtr inc_frag1;
    unsigned int inc1_1_len = utils::pack(inc_frag1, requestID, raw1_len + raw1_2_len, 0, raw1_len, raw1_1.get());

    BytePtr inc_frag2;
    unsigned int inc1_2_len = utils::pack(inc_frag2, requestID, raw1_len + raw1_2_len, 1, raw1_2_len, raw1_2.get());

    BytePtr msg2;
    unsigned int msg2_len = utils::pack(msg2, 1, raw2_len, 0, raw2_len, raw2.get());

    EXPECT_CALL(mock_server, receive_msg_impl)
            .WillOnce(DoAll(
                    SetArrayArgument<0>(inc_frag1.get(), inc_frag1.get() + inc1_1_len),
                    Return(inc1_1_len)))
            .WillOnce(DoAll(
                    SetArrayArgument<0>(msg2.get(), msg2.get() + msg2_len),
                    Return(msg2_len)))
            .WillOnce(DoAll(
                    SetArrayArgument<0>(inc_frag2.get(), inc_frag2.get() + inc1_2_len),
                    Return(inc1_2_len)));
    EXPECT_CALL(mock_server, get_client_address)
            .WillOnce(ReturnRef(client1))
            .WillOnce(ReturnRef(client1))
            .WillOnce(ReturnRef(client1));

    BytePtr er_raw; //expected reply raw
    unsigned int er_raw_len = utils::pack(er_raw, constants::SUCCESS);

    EXPECT_CALL(mock_handler, send_complete_message(_, _, _, requestID, client1))
            .With(Args<1, 2>(ElementsAreArray(er_raw.get(), er_raw_len)));

    mock_handler.receive_handle_message(mock_server, constants::ATLEAST);

    std::string test_file_actual_content = utils::read_file_to_string(constants::FILE_DIR_PATH + "test_file");
    EXPECT_EQ(test_file_actual_content, to_insert.append(test_rsc::test_file_content));

    delete[] buffer;
}

TEST(fragments, wrong_fragment_wrong_sender) {
    MockHandler mock_handler;
    MockUdpServer_linux mock_server;
    prepare_file();

    sockaddr_storage client1 = get_client(1);
    sockaddr_storage client2 = get_client(2);
    unsigned int requestID = 0;
    std::string to_insert{};

    for (int i = 1; i <= constants::MAX_CONTENT_SIZE + 20; ++i) {
        to_insert.append("o");
        if (i % 30 == 0) to_insert.append("\n");
    }


    unsigned int first_string_len = constants::MAX_CONTENT_SIZE - 4 - 4 - 13 - 4;
    //str_length, offset, file string, service no
    auto overall_string_len = (unsigned int) to_insert.length();
    unsigned int sec_string_len = overall_string_len - first_string_len;

    auto *buffer = new unsigned char[first_string_len];
    memcpy(buffer, to_insert.c_str(), first_string_len);

    BytePtr raw1_1;
    unsigned int raw1_len = utils::pack(raw1_1, 2, std::string("test_file"), 0, overall_string_len,
                                        first_string_len, buffer);

    EXPECT_EQ(constants::MAX_CONTENT_SIZE, raw1_len); //first packet should be full

    memcpy(buffer, to_insert.c_str() + first_string_len, sec_string_len);

    BytePtr raw1_2;
    unsigned int raw1_2_len = utils::pack(raw1_2, sec_string_len, buffer);

    BytePtr raw2;
    unsigned int raw2_len = utils::pack(raw2, 2, std::string("test_file"), 0, std::string{"should not end up in file"});

    BytePtr inc_frag1;
    unsigned int inc1_1_len = utils::pack(inc_frag1, requestID, raw1_len + raw1_2_len, 0, raw1_len, raw1_1.get());

    BytePtr inc_frag2;
    unsigned int inc1_2_len = utils::pack(inc_frag2, requestID, raw1_len + raw1_2_len, 1, raw1_2_len, raw1_2.get());

    BytePtr msg2;
    unsigned int msg2_len = utils::pack(msg2, requestID, raw2_len, 0, raw2_len, raw2.get());

    EXPECT_CALL(mock_server, receive_msg_impl)
            .WillOnce(DoAll(
                    SetArrayArgument<0>(inc_frag1.get(), inc_frag1.get() + inc1_1_len),
                    Return(inc1_1_len)))
            .WillOnce(DoAll(
                    SetArrayArgument<0>(msg2.get(), msg2.get() + msg2_len),
                    Return(msg2_len)))
            .WillOnce(DoAll(
                    SetArrayArgument<0>(inc_frag2.get(), inc_frag2.get() + inc1_2_len),
                    Return(inc1_2_len)));
    EXPECT_CALL(mock_server, get_client_address)
            .WillOnce(ReturnRef(client1))
            .WillOnce(ReturnRef(client2))
            .WillOnce(ReturnRef(client1));

    BytePtr er_raw; //expected reply raw
    unsigned int er_raw_len = utils::pack(er_raw, constants::SUCCESS);

    BytePtr er2_raw;
    unsigned er2_raw_len = utils::pack(er2_raw, constants::SERVER_BUSY);

    EXPECT_CALL(mock_handler, send_complete_message(_, _, _, requestID, client2))
            .With(Args<1, 2>(ElementsAreArray(er2_raw.get(), er2_raw_len)));

    EXPECT_CALL(mock_handler, send_complete_message(_, _, _, requestID, client1))
            .With(Args<1, 2>(ElementsAreArray(er_raw.get(), er_raw_len)));

    mock_handler.receive_handle_message(mock_server, constants::ATLEAST);

    std::string test_file_actual_content = utils::read_file_to_string(constants::FILE_DIR_PATH + "test_file");
    EXPECT_EQ(test_file_actual_content, to_insert.append(test_rsc::test_file_content));

    delete[] buffer;
}

/**
 * Assumes that 3 Cache block sizes > max content size
 */
TEST(fragments, outgoing) {
    Handler handler;
    MockUdpServer_linux mock_server;
    prepare_file();

    sockaddr_storage client1 = get_client(1);
    unsigned int requestID = 0;
    std::string file_content{};

    for (int i = 1; i <= 3 * constants::CACHE_BLOCK_SIZE; ++i) {
        file_content.append("o");
    }

    utils::remove_content_from_file(constants::FILE_DIR_PATH + "test_file");
    utils::insert_to_file(constants::FILE_DIR_PATH + "test_file", file_content, 0);

    BytePtr raw1;
    unsigned int raw1_len = utils::pack(raw1, 1, std::string("test_file"), 0, 3 * constants::CACHE_BLOCK_SIZE);

    BytePtr inc_frag1;
    unsigned int inc1_len = utils::pack(inc_frag1, requestID, raw1_len, 0, raw1_len, raw1.get());

    EXPECT_CALL(mock_server, receive_msg_impl)
            .WillOnce(DoAll(
                    SetArrayArgument<0>(inc_frag1.get(), inc_frag1.get() + inc1_len),
                    Return(inc1_len)));
    EXPECT_CALL(mock_server, get_client_address)
            .WillOnce(ReturnRef(client1));


    unsigned int first_string_len = constants::MAX_CONTENT_SIZE - 4 - 4;
    auto overall_string_len = (unsigned int) file_content.length();
    unsigned int sec_string_len = overall_string_len - first_string_len;

    auto *buffer = new unsigned char[first_string_len];
    memcpy(buffer, file_content.c_str(), first_string_len);
    BytePtr raw_frag1;
    unsigned int raw_frag1_len = utils::pack(raw_frag1, overall_string_len, first_string_len, buffer);

    memcpy(buffer, file_content.c_str() + first_string_len, sec_string_len);

    BytePtr raw_frag2;
    unsigned int raw_frag2_len = utils::pack(raw_frag2, sec_string_len, buffer);

    BytePtr msg_frag1;
    unsigned int msg_frag1_len = utils::pack(msg_frag1, requestID, overall_string_len + 8, 0, constants::SUCCESS,
                                             raw_frag1_len, raw_frag1.get());

    EXPECT_EQ(constants::MAX_PACKET_SIZE, msg_frag1_len);

    BytePtr msg_frag2;
    unsigned int msg_frag2_len = utils::pack(msg_frag2, requestID, overall_string_len + 8, 1, raw_frag2_len,
                                             raw_frag2.get());

    EXPECT_CALL(mock_server, send_packet(_, _, client1))
            .With(Args<0, 1>(ElementsAreArray(msg_frag1.get(), msg_frag1_len)));
    EXPECT_CALL(mock_server, send_packet(_, _, client1))
            .With(Args<0, 1>(ElementsAreArray(msg_frag2.get(), msg_frag2_len)));

    handler.receive_handle_message(mock_server, constants::ATLEAST);

    delete[] buffer;
}

//
// Created by towa-ubuntu on 07.04.20.
//

/**This file tests at-most semantics
 * All tests follow the scenario that
 *      request and reply fit in one packet
 *      all operations can be executed successfully (always status=SUCCESS)
 */

#include "../test_resoures.hpp"
#include <chrono>
#include <thread>

TEST(Atmost, receive_ack) {
    MockHandler mock_handler;
    MockUdpServer_linux mock_server;
    prepare_file();

    sockaddr_storage client1 = get_client(1);
    sockaddr_storage client2 = get_client(2);
    unsigned int requestID = 0;
    std::string to_insert{"I want to insert this!"};

    BytePtr raw1;
    unsigned int raw1_len = utils::pack(raw1, 2, std::string("test_file"), 0, to_insert);

    BytePtr inc_msg1;
    unsigned int inc1_len = utils::pack(inc_msg1, requestID, raw1_len, 0, raw1_len, raw1.get());

    BytePtr ack;
    unsigned int ack_len = utils::pack(ack, requestID, 4, 0, 7);

    EXPECT_CALL(mock_server, receive_msg_impl)
            .WillOnce(DoAll(
                    SetArrayArgument<0>(inc_msg1.get(), inc_msg1.get() + inc1_len),
                    Return(inc1_len)))
            .WillOnce(DoAll(
                    SetArrayArgument<0>(ack.get(), ack.get() + ack_len),
                    Return(ack_len)))
            .WillOnce(DoAll(
                    SetArrayArgument<0>(inc_msg1.get(), inc_msg1.get() + inc1_len),
                    Return(inc1_len)));
    EXPECT_CALL(mock_server, get_client_address)
            .WillOnce(ReturnRef(client1))
            .WillOnce(ReturnRef(client1))
            .WillOnce(ReturnRef(client2));

    BytePtr er_raw; //expected reply raw
    unsigned int er_raw_len = utils::pack(er_raw, constants::SUCCESS);

    EXPECT_CALL(mock_handler, send_complete_message(_, _, _, requestID, client1))
            .With(Args<1, 2>(ElementsAreArray(er_raw.get(), er_raw_len)));
    EXPECT_CALL(mock_handler, send_complete_message(_, _, _, requestID, client2))
            .With(Args<1, 2>(ElementsAreArray(er_raw.get(), er_raw_len)));

    mock_handler.receive_handle_message(mock_server, constants::ATMOST);
    mock_handler.receive_handle_message(mock_server, constants::ATMOST);
}

/**
 * receive an ack that is not stored anymore, i.e. should be ignored
 */
TEST(Atmost, receive_wrong_ack) {
    MockHandler mock_handler;
    MockUdpServer_linux mock_server;
    prepare_file();

    sockaddr_storage client1 = get_client(1);
    sockaddr_storage client2 = get_client(2);
    unsigned int requestID = 0;
    std::string to_insert{"I want to insert this!"};

    BytePtr raw1;
    unsigned int raw1_len = utils::pack(raw1, 2, std::string("test_file"), 0, to_insert);

    BytePtr inc_msg1;
    unsigned int inc1_len = utils::pack(inc_msg1, requestID, raw1_len, 0, raw1_len, raw1.get());

    BytePtr ack;
    unsigned int ack_len = utils::pack(ack, 1, 4, 0, 7);

    EXPECT_CALL(mock_server, receive_msg_impl)
            .WillOnce(DoAll(
                    SetArrayArgument<0>(inc_msg1.get(), inc_msg1.get() + inc1_len),
                    Return(inc1_len)))
            .WillOnce(DoAll(
                    SetArrayArgument<0>(ack.get(), ack.get() + ack_len),
                    Return(ack_len)))
            .WillOnce(DoAll(
                    SetArrayArgument<0>(inc_msg1.get(), inc_msg1.get() + inc1_len),
                    Return(inc1_len)));
    EXPECT_CALL(mock_server, get_client_address)
            .WillOnce(ReturnRef(client1))
            .WillOnce(ReturnRef(client1))
            .WillOnce(ReturnRef(client2));

    BytePtr er_raw; //expected reply raw
    unsigned int er_raw_len = utils::pack(er_raw, constants::SUCCESS);

    EXPECT_CALL(mock_handler, send_complete_message(_, _, _, requestID, client1))
            .With(Args<1, 2>(ElementsAreArray(er_raw.get(), er_raw_len)));
    EXPECT_CALL(mock_handler, send_complete_message(_, _, _, requestID, client2))
            .With(Args<1, 2>(ElementsAreArray(er_raw.get(), er_raw_len)));

    mock_handler.receive_handle_message(mock_server, constants::ATMOST);
    mock_handler.receive_handle_message(mock_server, constants::ATMOST);
}

/**
 * Set ACK_TIMEOUT < 12s for this test, or adjust sleep time
 */
TEST(Atmost, resend) {
    MockHandler mock_handler;
    MockUdpServer_linux mock_server;
    prepare_file();

    sockaddr_storage client1 = get_client(1);
    sockaddr_storage client2 = get_client(2);
    unsigned int requestID = 0;
    std::string to_insert{"I want to insert this!"};

    BytePtr raw1;
    unsigned int raw1_len = utils::pack(raw1, 2, std::string("test_file"), 0, to_insert);

    BytePtr inc_msg1;
    unsigned int inc1_len = utils::pack(inc_msg1, requestID, raw1_len, 0, raw1_len, raw1.get());

    EXPECT_CALL(mock_server, receive_msg_impl)
            .WillOnce(DoAll(
                    SetArrayArgument<0>(inc_msg1.get(), inc_msg1.get() + inc1_len),
                    Return(inc1_len)))
            .WillOnce(Return(UdpServer_linux::TIMEOUT))
            .WillOnce(DoAll(
                    SetArrayArgument<0>(inc_msg1.get(), inc_msg1.get() + inc1_len),
                    Return(inc1_len)));
    EXPECT_CALL(mock_server, get_client_address)
            .WillOnce(ReturnRef(client1))
            .WillOnce(ReturnRef(client2));

    BytePtr er_raw; //expected reply raw
    unsigned int er_raw_len = utils::pack(er_raw, constants::SUCCESS);

    EXPECT_CALL(mock_handler, send_complete_message(_, _, _, requestID, client1))
            .With(Args<1, 2>(ElementsAreArray(er_raw.get(), er_raw_len)))
            .Times(2);
    EXPECT_CALL(mock_handler, send_complete_message(_, _, _, requestID, client2))
            .With(Args<1, 2>(ElementsAreArray(er_raw.get(), er_raw_len)));

    mock_handler.receive_handle_message(mock_server, constants::ATMOST);
    std::cout << "Let Thread sleep for 10s" << std::endl;
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(12s);
    mock_handler.receive_handle_message(mock_server, constants::ATMOST);
    mock_handler.receive_handle_message(mock_server, constants::ATMOST);
}

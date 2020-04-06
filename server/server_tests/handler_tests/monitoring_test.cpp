//
// Created by towa-ubuntu on 06.04.20.
//

/**This file tests 'Monitor'
 * All tests follow the scenario that
 *      request and reply fit in one packet
 *      all operations can be executed successfully (always status=SUCCESS)
 *      based on handler_overall_tests
 */

#include "test_resoures.hpp"

TEST(Monitor, remove_content) {
    MockHandler mock_handler;
    MockUdpServer_linux mock_server;
    prepare_file();

    sockaddr_storage client1 = get_client(1);
    sockaddr_storage client2 = get_client(2);
    unsigned int requestID = 0;

    BytePtr raw1;
    unsigned int raw1_len = utils::pack(raw1, 4, std::string("test_file"));

    BytePtr inc_msg1;
    unsigned int inc1_len = utils::pack(inc_msg1, requestID, raw1_len, 0, raw1_len, raw1.get());

    BytePtr raw2;
    unsigned int raw2_len = utils::pack(raw2, 3, std::string("test_file"), 600000);

    BytePtr inc_msg2;
    unsigned int inc2_len = utils::pack(inc_msg2, requestID, raw2_len, 0, raw2_len, raw2.get());

    EXPECT_CALL(mock_server, receive_msg_impl)
            .WillOnce(DoAll(
                    SetArrayArgument<0>(inc_msg2.get(), inc_msg2.get() + inc2_len),
                    Return(inc2_len)))
            .WillOnce(DoAll(
                    SetArrayArgument<0>(inc_msg1.get(), inc_msg1.get() + inc1_len),
                    Return(inc1_len)
            ));
    EXPECT_CALL(mock_server, get_client_address)
            .WillOnce(ReturnRef(client2))
            .WillOnce(ReturnRef(client1));

    BytePtr er_raw1; //expected reply raw
    unsigned int er_raw1_len = utils::pack(er_raw1, constants::SUCCESS);

    BytePtr er_raw2;
    unsigned int er_raw2_len = utils::pack(er_raw2, constants::FILE_WAS_MODIFIED, std::string("test_file"),
                                           std::string{""});

    EXPECT_CALL(mock_handler, send_complete_message(_, _, _, requestID, client2))
            .With(Args<1, 2>(ElementsAreArray(er_raw1.get(), er_raw1_len)));

    EXPECT_CALL(mock_handler, send_complete_message(_, _, _, requestID, client2))
            .With(Args<1, 2>(ElementsAreArray(er_raw2.get(), er_raw2_len)));

    EXPECT_CALL(mock_handler, send_complete_message(_, _, _, requestID, client1))
            .With(Args<1, 2>(ElementsAreArray(er_raw1.get(), er_raw1_len)));

    mock_handler.receive_handle_message(mock_server, constants::ATLEAST);
    mock_handler.receive_handle_message(mock_server, constants::ATLEAST);

    std::string test_file_actual_content = utils::read_file_to_string(constants::FILE_DIR_PATH + "test_file");
    EXPECT_EQ(test_file_actual_content, std::string{""});
}

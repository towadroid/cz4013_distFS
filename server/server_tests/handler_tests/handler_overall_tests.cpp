//
// Created by towa-ubuntu on 05.04.20.
//

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "../mock_classes/MockHandler.hpp"
#include "../mock_classes/MockUdpServer_linux.hpp"
#include "../../utils/packing.hpp"
#include "test_resoures.hpp"
#include "../../HelperClasses.hpp"

using testing::SetArrayArgument;
using testing::ReturnRef;
using testing::DoAll;
using testing::Return;
using testing::Args;
using testing::ElementsAreArray;
using testing::_;
using testing::WithArg;

TEST(Handler_overall, read) {
    MockHandler mock_handler;
    MockUdpServer_linux mock_server;

    sockaddr_storage client1 = get_client(1);
    unsigned int requestID = 0;

    BytePtr raw1;
    unsigned int raw1_len = utils::pack(raw1, 1, std::string("file1"), 0, 5);

    BytePtr inc_msg1;
    unsigned int inc1_len = utils::pack(inc_msg1, requestID, raw1_len, 0, raw1_len, raw1.get());

    EXPECT_CALL(mock_server, receive_msg_impl)
            .WillOnce(DoAll(
                    SetArrayArgument<0>(inc_msg1.get(), inc_msg1.get() + inc1_len),
                    Return(inc1_len)
                      )
            );
    EXPECT_CALL(mock_server, get_client_address).WillRepeatedly(ReturnRef(client1));

    std::string file1_content{"01234567890123456789012345678901234567890123456789\n"
                              "\n"
                              "This is read only content!\n"};
    BytePtr er_raw; //expected reply raw
    unsigned int er_raw_len = utils::pack(er_raw, constants::SUCCESS, file1_content);

    EXPECT_CALL(mock_handler, send_complete_message(_, _, _, requestID, client1))
            .With(Args<1, 2>(ElementsAreArray(er_raw.get(), er_raw_len)));

    mock_handler.receive_handle_message(mock_server, constants::ATLEAST);
}

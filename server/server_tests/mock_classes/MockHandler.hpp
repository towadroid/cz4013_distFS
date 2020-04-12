//
// Created by towa-ubuntu on 3/18/20.
//

#ifndef SERVER_MOCKHANDLER_HPP
#define SERVER_MOCKHANDLER_HPP

#include "../../Handler.hpp"

using ::testing::_;
using ::testing::Return;

class MockHandler : public Handler {
public:
    MockHandler() : Handler() {
        ON_CALL(*this, send_complete_message(_, _, _, _, _))
                .WillByDefault((Return(true)));
    } // https://stackoverflow.com/a/16553559/13147660

    MOCK_METHOD(bool, send_complete_message, (UdpServer_linux & server, unsigned char * raw_content_buf, size_t
            len, unsigned int
            requestID,
            const sockaddr_storage &receiver), (override));

    /*MOCK_METHOD(void, service, (Service_type
            service_type,
            const UdpServer_linux &server,
            unsigned char *complete_raw_content, BytePtr
            &raw_reply, unsigned int & raw_reply_length), (override));*/
};

#endif //SERVER_MOCKHANDLER_HPP

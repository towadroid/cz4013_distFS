//
// Created by towa-ubuntu on 3/19/20.
//

#ifndef SERVER_MOCKUDPSERVER_LINUX_HPP
#define SERVER_MOCKUDPSERVER_LINUX_HPP

#include "gmock/gmock.h"
#include "../../UdpServer_linux.hpp"

class MockUdpServer_linux : public UdpServer_linux {
public:
    MOCK_METHOD(int, receive_msg_impl, (unsigned char * buf, int
            sec, int
            usec));

    MOCK_METHOD(const sockaddr_storage &, get_client_address, (), (const, override));

    MOCK_METHOD(int, send_packet, (const unsigned char *buf, size_t len, const sockaddr_storage &receiver),
                (override));

    MockUdpServer_linux() : UdpServer_linux(2302) {}

    ///https://stackoverflow.com/a/45049665
    virtual int receive_msg(unsigned char *buf, int sec = -1, int usec = -1) override {
        return receive_msg_impl(buf, sec, usec);
    }

};


#endif //SERVER_MOCKUDPSERVER_LINUX_HPP

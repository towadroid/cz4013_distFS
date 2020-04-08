//
// Created by towa-ubuntu on 08.04.20.
//

#include "test_resoures.hpp"

TEST(send_failure, deterministic) {
    UdpServer_linux server(2302, 0.5, 1);
    unsigned char buffer[4];
    sockaddr_storage client1 = get_client(1);
    int count_failed = 0;
    int status;
    for (int i = 0; i < 100; ++i) {
        status = server.send_packet(buffer, 4, client1);
        if (status == UdpServer_linux::SIMULATED_FAILURE) count_failed++;
    }
    //should be exactly 47
    EXPECT_GE(count_failed, 45);
    EXPECT_LE(count_failed, 55);
}

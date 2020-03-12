//
// Created by towa-ubuntu on 2/28/20.
//

#include <iostream>
#include "constants.hpp"
#include "UdpServer_linux.hpp"
#include "utils.hpp"
#include "Handler.hpp"
#include "MonitoringClient.hpp"
/*
#include <thread>
#include <chrono>
std::this_thread::sleep_for(std::chrono::milliseconds(100)); // sleep for x ms*/

using std::cout;
using std::endl;

void readfile() {
    std::string tfile;
    utils::read_file_to_string(constants::FILE_DIR_PATH + "file1", &tfile);
    cout << tfile;
}

void insertfile() {
    utils::insert_to_file(constants::FILE_DIR_PATH + "test_ins", "abc", 2);
}

// waits for an incoming message and upon receipt answers with a message to the client
void testUdp() {
    UdpServer_linux serv{2302};
    unsigned char buffer[1024];
    int n = serv.receive_msg(buffer);
    buffer[n] = '\0';
    printf("Client: %s\n", buffer);
    std::string msg("Hello from server");
    utils::pack_str(buffer, msg);
    int len = 2 + msg.length();
    //std::cout <<  client_address.sin_port;
    serv.send_msg(buffer, len);
}

void executeHandler(){
    UdpServer_linux server(2302);
    unsigned char buffer[1024];
    Handler h{};
    h.service(constants::service_codes.at(1), server, buffer);
}

int main(int argc, char **argv) {
    //readfile();
    //insertfile();
    //testUdp();
    //executeHandler();
    UdpServer_linux server(2302);
    unsigned char buffer[1024];
    Handler h{};

}

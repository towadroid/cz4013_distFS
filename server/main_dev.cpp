//
// Created by towa-ubuntu on 2/28/20.
//

#include <iostream>
#include "constants.hpp"
#include "UdpServer_linux.hpp"
#include "utils/utils.hpp"
#include "utils/packing.hpp"
#include "Handler.hpp"
#include "MonitoringClient.hpp"
#include "spdlog/spdlog.h"
#include "spdlog/logger.h"
#include "HelperClasses.hpp"

#include <chrono>
#include "HelperClasses.hpp"
/*
#include <thread>
std::this_thread::sleep_for(std::chrono::milliseconds(100)); // sleep for x ms*/


using std::cout;
using std::endl;

void readfile() {
    std::string tfile = utils::read_file_to_string(constants::FILE_DIR_PATH + "file1");
    cout << tfile;
}

void readfile_cached() {
    std::string a;
    utils::read_file_to_string_cached(constants::FILE_DIR_PATH + "file1", 2, 52);
    cout << a << endl;
}

void insertfile() {
    utils::insert_to_file(constants::FILE_DIR_PATH + "test_ins", "abc", 2);
}

// waits for an incoming message and upon receipt answers with a message to the client
void testUdp(int sec = -1, int usec = -1) {
    UdpServer_linux serv{2302};
    unsigned char buffer[1024];
    int n = serv.receive_msg(buffer, sec, usec);
    buffer[n] = '\0';
    printf("Client: %s\n", buffer);
    std::string msg("Hello from server");
    BytePtr packed;
    utils::pack(packed, msg);
    size_t len = 4 + msg.length();
    //std::cout <<  client_address.sin_port;
    serv.send_packet(packed.get(), len);
}

void executeHandler() {
    UdpServer_linux server(2302);
    unsigned char buffer[1024];
    Handler h{};
    BytePtr raw_reply;
    unsigned int len;
    h.service(constants::service_codes.at(1), server, buffer, raw_reply, len);
}

void logger() {
    spdlog::set_level(spdlog::level::trace);
    spdlog::trace("Trace");
    spdlog::debug("Debug");
    spdlog::info("Welcome to spdlog!\nWith linebreak");
    spdlog::warn("Easy padding in numbers like {:08d}", 12);
    spdlog::error("Error");
    spdlog::critical("das ist kritisch");
    printf("%d", spdlog::default_logger()->level());
}

int main() {
    spdlog::set_level(spdlog::level::trace);
    //readfile();
    //readfile_cached();
    //insertfile();
    //testUdp();
    //testUdp(5, 0);
    //executeHandler();
    //logger();
    UdpServer_linux server(2302);
    unsigned char buffer[1024];
    buffer[0] = 3;
    buffer[0] = buffer[0];
    Handler h{};
    std::unordered_map<sockaddr_storage, int, SockaddrStor_Hasher, SockaddrStor_Equal> um{};
}

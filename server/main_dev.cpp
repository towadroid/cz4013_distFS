//
// Created by towa-ubuntu on 2/28/20.
//

#include <iostream>
#include "constants.hpp"
#include "UdpServer_linux.hpp"
#include "utils.hpp"

using std::cout;
using std::endl;

void readfile(){
    std::string tfile;
    utils::read_file_to_string(constants::FILE_DIR_PATH+"file1", &tfile);
    cout << tfile;
}

void insertfile(){
    utils::insert_to_file(constants::FILE_DIR_PATH+"test_ins","abc",2);
}

int main(int argc, char **argv) {
    //readfile();
    //insertfile();
    UdpServer_linux serv(2302);
    unsigned char buffer[1024];
    struct sockaddr_storage client_address;
    int n = serv.receive_msg(buffer, &client_address);
    buffer[n] = '\0';
    printf("Client: %s\n", buffer);
    std::string msg("Hello from server");
    utils::pack_str(buffer, msg);
    int len = 2+ msg.length();
    //std::cout <<  client_address.sin_port;
    serv.send_msg(buffer, len, &client_address);
}

//
// Created by Tobias on 23.02.2020.
//

#ifndef SERVER_UDPSERVER_HPP
#define SERVER_UDPSERVER_HPP

#include <winsock2.h>
#include "constants.hpp"

class UdpServer {
public:
    explicit UdpServer(int portno, double failure_rate); // use explicit, see https://stackoverflow.com/a/121163
    ~UdpServer();

private:
    SOCKET ListenSocket;
    int portno; //port number
    double failure_rate;
};


#endif //SERVER_UDPSERVER_HPP

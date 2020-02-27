//
// Created by Tobias on 23.02.2020.
//

#include "UdpServer.hpp"
#include <iostream>
#include <cstdlib> //for exit() function
#include <ws2tcpip.h> //to get type addrinfo
#include "globalvar.hpp"

//Use initiliazer list
UdpServer::UdpServer(int portno, double failure_rate) : portno(portno), failure_rate(failure_rate) {
    //See https://docs.microsoft.com/en-us/windows/win32/winsock/initializing-winsock
    WSADATA wsa_data;

    //initialize Winsock
    if (verbose_flag) std::cout << "Initializing winsock" << std::endl;
        int result_val = WSAStartup(MAKEWORD(2, 2), &wsa_data);
    if (0 != result_val) {
        std::cerr << "WSAStartup failed with error: " << result_val << std::endl;
        exit(1);
    }

    //preparation to create a socket
    struct addrinfo *result = nullptr, *ptr = nullptr, hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET; //use IPv4
    hints.ai_socktype = SOCK_DGRAM; //use datagrams
    hints.ai_protocol = IPPROTO_UDP; //use UDP
    hints.ai_flags = AI_PASSIVE;
    /*flag indicates the caller intends to use the returned socket address structure
     * in a call to the bind function*/

    // change portno to char*
    std::string port = std::to_string(portno);
    char const *p_port = port.c_str();
    // Resolve the local address and port to be used by the server
    result_val = getaddrinfo(nullptr, p_port, &hints, &result);
    if (0 != result_val) {
        std::cerr << "getaddrinfo failed with error: " << result_val << std::endl;
        WSACleanup();
        exit(1);
    }

    //initialize a socket for the server
    ListenSocket = INVALID_SOCKET;

    // Create a SOCKET for the server to listen for client connections
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    if (ListenSocket == INVALID_SOCKET) {
        std::cerr << "Error at socket(): " << WSAGetLastError() << std::endl;
        freeaddrinfo(result);
        WSACleanup();
        exit(1);
    }

    // Setup the TCP listening socket
    result_val = bind(ListenSocket, result->ai_addr, (int) result->ai_addrlen);
    if (result_val == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        exit(1);
    }

    freeaddrinfo(result);
}

UdpServer::~UdpServer() {
//TODO close socket?
}

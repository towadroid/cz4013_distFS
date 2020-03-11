//
// Created by towa-ubuntu on 27/2/20.
//

#ifndef SERVER_UDPSERVER_LINUX_HPP
#define SERVER_UDPSERVER_LINUX_HPP

#include <sys/socket.h>
#include <netinet/in.h>

class UdpServer_linux {
private:
    int sockfd; //descriptor for socket
    int portno; //port to listen tog

    struct sockaddr_in server_address;
    // pointer to this can be cast to a pointer to struct sockaddr


public:
    explicit UdpServer_linux(int portno);
    ~UdpServer_linux();
    int receive_msg(unsigned char *buf, sockaddr_storage *client_address);
    void send_msg(const unsigned char *buf, int len, sockaddr_storage *client_address);
};


#endif //SERVER_UDPSERVER_LINUX_HPP

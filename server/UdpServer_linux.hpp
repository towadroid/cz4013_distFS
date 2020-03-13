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
    double failure_rate;

    struct sockaddr_storage client_address{};


public:
    explicit UdpServer_linux(int portno, double failure_rate  = 0);
    ~UdpServer_linux();
    int receive_msg(unsigned char *buf);
    void send_msg(const unsigned char *buf, size_t len) const;
    void send_msg(const unsigned char *buf, size_t len, const sockaddr_storage *receiver) const;
    sockaddr_storage get_client_address() const;
};


#endif //SERVER_UDPSERVER_LINUX_HPP

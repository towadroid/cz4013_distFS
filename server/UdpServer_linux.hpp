//
// Created by towa-ubuntu on 27/2/20.
//

#ifndef SERVER_UDPSERVER_LINUX_HPP
#define SERVER_UDPSERVER_LINUX_HPP

#include <sys/socket.h>
#include <netinet/in.h>

constexpr int TIMEOUT = -5;
constexpr int FUNCTION_ERROR = -10;

class UdpServer_linux {
private:
    int sockfd; //descriptor for socket
    int portno; //port to listen tog
    double failure_rate;

    struct sockaddr_storage client_address{};


public:
    explicit UdpServer_linux(int portno, double failure_rate = 0);

    virtual ~UdpServer_linux();

    [[nodiscard]]  virtual int receive_msg(unsigned char *buf, int sec = -1, int usec = -1);

    void send_packet(const unsigned char *buf, size_t len) const;

    void send_packet(const unsigned char *buf, size_t len, const sockaddr_storage *receiver) const;

    virtual const sockaddr_storage &get_client_address() const;
};


#endif //SERVER_UDPSERVER_LINUX_HPP

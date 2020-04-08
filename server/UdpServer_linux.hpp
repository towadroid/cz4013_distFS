//
// Created by towa-ubuntu on 27/2/20.
//

#ifndef SERVER_UDPSERVER_LINUX_HPP
#define SERVER_UDPSERVER_LINUX_HPP

#include <sys/socket.h>
#include <netinet/in.h>
#include <random>


class UdpServer_linux {
private:
    int sockfd; //descriptor for socket
    int portno; //port to listen tog
    double failure_rate;
    std::mt19937 random_generator;
    std::uniform_real_distribution<double> dist;

    struct sockaddr_storage client_address{};


public:
    static constexpr int TIMEOUT = -5;
    static constexpr int FUNCTION_ERROR = -10;
    static constexpr int SIMULATED_FAILURE = -15;

    explicit UdpServer_linux(int portno, double failure_rate = 0, unsigned int seed = 1);

    UdpServer_linux(int portno, double failure_rate, unsigned int seed, bool truly_random);

    virtual ~UdpServer_linux();

    [[nodiscard]]  virtual int receive_msg(unsigned char *buf, int sec = -1, int usec = -1);

    void send_packet(const unsigned char *buf, size_t len);

    virtual int send_packet(const unsigned char *buf, size_t len, const sockaddr_storage &receiver);

    virtual const sockaddr_storage &get_client_address() const;
};


#endif //SERVER_UDPSERVER_LINUX_HPP

//
// Created by towa-ubuntu on 27/2/20.
//

#include <netdb.h>
#include <cstring> //for memset
#include <string>
#include <stdexcept>
#include <unistd.h> //for close
#include "spdlog/spdlog.h"
#include "spdlog/fmt/bin_to_hex.h"
#include <arpa/inet.h> //for inet_ntop
#include "utils.hpp"
#include "UdpServer_linux.hpp"

UdpServer_linux::UdpServer_linux(int portno, double failure_rate) : portno(portno), failure_rate(failure_rate) {
    struct addrinfo hints{};
    int status;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;          //specify IPv4
    hints.ai_socktype = SOCK_DGRAM;     //specify datagram
    hints.ai_flags = AI_PASSIVE;        //use my own IP

    std::string portno_string = std::to_string(portno);
    struct addrinfo *servinfo;
    status = getaddrinfo(nullptr, portno_string.c_str(), &hints, &servinfo);
    if (status != 0) {
        std::string exc = "getaddrinfo: ";
        exc.append(gai_strerror(status));
        throw std::runtime_error(exc);
    }

    // loop through all the results and bind to the first we can
    struct addrinfo *p;
    for (p = servinfo; nullptr != p; p = p->ai_next) {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd < 0) {
            perror("server: open socket failed");
            continue;
        }
        int yes = 1;
        // avoid "Address already in use" error message
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }
        break;
    }
    freeaddrinfo(servinfo);

    if (nullptr == p) throw std::runtime_error("server: failed to bind");


}

UdpServer_linux::~UdpServer_linux() {
    close(sockfd);
}

int UdpServer_linux::receive_msg(unsigned char *buf) {
    //TODO fix hardcode 1024
    socklen_t addr_len = sizeof(client_address); //not used later?
    int numbytes = recvfrom(sockfd, buf, 1024, MSG_WAITALL, (struct sockaddr *) &client_address, &addr_len);
    if (-1 == numbytes) perror("recvfrom");

    //logging purpose
    {
        char s[INET6_ADDRSTRLEN];
        spdlog::debug("received packet from {} containing {} bytes",
                      inet_ntop(client_address.ss_family,
                                utils::get_in_addr((struct sockaddr *) &client_address),
                                s, sizeof s), numbytes);
        spdlog::trace("packet content: {:X}", spdlog::to_hex(buf, buf + numbytes));
    }
    return numbytes;
}

void UdpServer_linux::send_msg(unsigned char const *buf, size_t len) const {
    send_msg(buf, len, &client_address);
}

sockaddr_storage UdpServer_linux::get_client_address() const {
    return client_address;
}

void UdpServer_linux::send_msg(const unsigned char *buf, size_t len, const sockaddr_storage *receiver) const {
    int n = sendto(sockfd, buf, len, MSG_CONFIRM, (const sockaddr *) receiver, sizeof(*receiver));
    if (n != len) {
        if (-1 == n) spdlog::error("send_msg");
        else spdlog::error("Could only send {} bytes out of {}", n, len);
    }
}

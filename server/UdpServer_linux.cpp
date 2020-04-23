//
// Created by towa-ubuntu on 27/2/20.
//

#include <netdb.h>
#include <cstring> //for memset
#include <string>
#include <stdexcept>
#include <unistd.h> //for close
#include <cstdlib> //for random
#include "spdlog/spdlog.h"
#include "spdlog/fmt/bin_to_hex.h"
#include <arpa/inet.h> //for inet_ntop
#include "utils/utils.hpp"
#include "UdpServer_linux.hpp"

UdpServer_linux::UdpServer_linux(int portno, double failure_rate, unsigned int seed) :
        portno(portno), failure_rate(failure_rate) {
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

    random_generator = std::mt19937(seed);
    dist = std::uniform_real_distribution<double>(0.0, 1.0);

}

/** Create server with the option of truly random failure behavior
 *
 * @param portno
 * @param failure_rate
 * @param seed
 * @param truly_random If true, ignore seed
 */
UdpServer_linux::UdpServer_linux(int portno, double failure_rate, unsigned int seed, bool truly_random) :
        UdpServer_linux(portno, failure_rate, seed) {
    if (truly_random) {
        auto seed2 = (unsigned long) std::chrono::system_clock::now().time_since_epoch().count();
        random_generator = std::mt19937(seed2);
    }
}

UdpServer_linux::~UdpServer_linux() {
    close(sockfd);
}

/**
 * returns TIMEOUT if timeout; FUNCTION_ERROR if something is wrong with the function
 * all other return values are from recvfrom
 * no timeout if either sec < 0 or usec < 0
 *
 * @param buf
 * @param sec
 * @param usec
 * @return
 */
int UdpServer_linux::receive_msg(unsigned char *buf, int sec, int usec) {
    fd_set readfds;

    // clear the set ahead of time
    FD_ZERO(&readfds);

    //add descriptor to the set
    FD_SET(sockfd, &readfds);

    int rv;

    if (sec < 0 || usec < 0) rv = select(sockfd + 1, &readfds, nullptr, nullptr, nullptr);
    else {
        // wait until either socket has data ready to be recvfrom()d
        struct timeval tv{sec, usec};
        rv = select(sockfd + 1, &readfds, nullptr, nullptr, &tv);
    }

    if (-1 == rv) perror("select");
    else if (0 == rv) {
        spdlog::trace("Timeout occurred! No data! (Note that we remove the entry from timeout list lazily)");
        return TIMEOUT;
    } else {
        socklen_t addr_len = sizeof(client_address);
        int numbytes = (int) recvfrom(sockfd, buf, 1024, MSG_WAITALL, (struct sockaddr *) &client_address, &addr_len);
        if (-1 == numbytes) perror("recvfrom");

        //logging purpose
        {
            spdlog::debug("received packet from {}:{} containing {} bytes",
                          utils::get_in_addr_str(&client_address),
                          utils::get_in_port(&client_address), numbytes);
            spdlog::trace("packet content: {:X}", spdlog::to_hex(buf, buf + numbytes));
        }
        return numbytes;
    }
    return FUNCTION_ERROR;

}

void UdpServer_linux::send_packet(unsigned char const *buf, size_t len) {
    send_packet(buf, len, client_address);
}

const sockaddr_storage &UdpServer_linux::get_client_address() const {
    return client_address;
}

int UdpServer_linux::send_packet(const unsigned char *buf, size_t len, const sockaddr_storage &receiver) {
    if (dist(random_generator) > failure_rate) {
        int n = (int) sendto(sockfd, buf, len, MSG_CONFIRM, (const sockaddr *) &receiver, sizeof(receiver));
        if (n != (int) len) {
            if (-1 == n) spdlog::error("send_packet");
            else spdlog::error("Could only send {} bytes out of {}", n, len);
        } else {
            spdlog::trace("Sent packet ({} bytes) to {} with content: {:X}", len, utils::get_in_addr_port_str(receiver),
                          spdlog::to_hex(buf, buf + len));
        }
        return 0;
    } else {
        spdlog::trace("Simulated failure: Packet ({} bytes) to {} with content: {:X} was not sent.",
                      len, utils::get_in_addr_port_str(receiver), spdlog::to_hex(buf, buf + len));
        return SIMULATED_FAILURE;
    }

}


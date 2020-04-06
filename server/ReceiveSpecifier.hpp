//
// Created by towa-ubuntu on 06.04.20.
//

#ifndef SERVER_RECEIVESPECIFIER_HPP
#define SERVER_RECEIVESPECIFIER_HPP

#include "constants.hpp"
#include "UdpServer_linux.hpp"
#include "MonitoringClient.hpp"

class ReceiveSpecifier {
public:
    ReceiveSpecifier(); //no timeout, no expectation
    explicit ReceiveSpecifier(unsigned int timeout_ms); //timeout, no expectation
    explicit ReceiveSpecifier(const TimeStamp &timeout_time); //timeout, no expectation
    ReceiveSpecifier(const sockaddr_storage &exp_address, unsigned int exp_requestID, unsigned int exp_fragment_no);

    ReceiveSpecifier(unsigned int timeout_ms, const sockaddr_storage &exp_address, unsigned int exp_request_ID,
                     unsigned int exp_fragment_no);

    ReceiveSpecifier(const TimeStamp &timeout_time, const sockaddr_storage &exp_address, unsigned int exp_request_ID,
                     unsigned int exp_fragment_no);

    bool is_specific();

    bool is_timeout();

    bool is_timeouted();

    void time_in_s_usec(int &s, int &usec);

    [[nodiscard]] const TimeStamp &getTimeoutTime() const;

    [[nodiscard]] const sockaddr_storage &getExpAddress() const;

    [[nodiscard]] unsigned int getExpRequestId() const;

    [[nodiscard]] unsigned int getExpFragmentNo() const;

    void setExpAddress(const sockaddr_storage &expAddress);

private:
    bool specific;
    bool has_timeout;
    sockaddr_storage exp_address{};
    unsigned int exp_requestID{};
    unsigned int exp_fragment_no{};
    TimeStamp timeout_time;
};


#endif //SERVER_RECEIVESPECIFIER_HPP

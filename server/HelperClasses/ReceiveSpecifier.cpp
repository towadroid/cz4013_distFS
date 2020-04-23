//
// Created by towa-ubuntu on 06.04.20.
//

#include "ReceiveSpecifier.hpp"

ReceiveSpecifier::ReceiveSpecifier() : specific(false), has_timeout(false) {
}

ReceiveSpecifier::ReceiveSpecifier(unsigned int timeout_ms) : specific(false), has_timeout(true) {
    timeout_time = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeout_ms);
}

ReceiveSpecifier::ReceiveSpecifier(const TimeStamp &timeout_time) :
        specific(false), has_timeout(true), timeout_time(timeout_time) {
}

ReceiveSpecifier::ReceiveSpecifier(const sockaddr_storage &exp_address, unsigned int exp_requestID,
                                   unsigned int exp_fragment_no) :
        specific(true), has_timeout(false), exp_address(exp_address), exp_requestID(exp_requestID),
        exp_fragment_no(exp_fragment_no) {
}

ReceiveSpecifier::ReceiveSpecifier(unsigned int timeout_ms, const sockaddr_storage &exp_address,
                                   unsigned int exp_requestID, unsigned int exp_fragment_no) :
        specific(true), has_timeout(true), exp_address(exp_address), exp_requestID(exp_requestID),
        exp_fragment_no(exp_fragment_no) {
    timeout_time = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeout_ms);
}

ReceiveSpecifier::ReceiveSpecifier(const TimeStamp &timeout_time, const sockaddr_storage &exp_address,
                                   unsigned int exp_requestID, unsigned int exp_fragment_no) :
        specific(true), has_timeout(true), exp_address(exp_address), exp_requestID(exp_requestID),
        exp_fragment_no(exp_fragment_no), timeout_time(timeout_time) {
}

bool ReceiveSpecifier::is_specific() {
    return specific;
}

const sockaddr_storage &ReceiveSpecifier::getExpAddress() const {
    return exp_address;
}

unsigned int ReceiveSpecifier::getExpRequestId() const {
    if (specific) return exp_requestID;
    throw std::runtime_error("Asked for expRequestID but specific is not set");
}

unsigned int ReceiveSpecifier::getExpFragmentNo() const {
    if (specific) return exp_fragment_no;
    throw std::runtime_error("Asked for expFragmentNo but specific is not set");
}

bool ReceiveSpecifier::is_timeout() {
    return has_timeout;
}

void ReceiveSpecifier::setExpAddress(const sockaddr_storage &expAddress) {
    exp_address = expAddress;
}

const TimeStamp &ReceiveSpecifier::getTimeoutTime() const {
    if (has_timeout) return timeout_time;
    throw std::runtime_error("Asked for timeout time but has no timeout time");
}

bool ReceiveSpecifier::is_timeouted() {
    if (has_timeout) return std::chrono::steady_clock::now() > timeout_time;
    else return false;
}

void ReceiveSpecifier::time_in_s_usec(int &s, int &usec) {
    if (has_timeout) {
        using namespace std::chrono;
        auto duration = timeout_time - steady_clock::now();
        s = (int) duration_cast<seconds>(duration).count();
        usec = (int) duration_cast<microseconds>(duration - seconds(s)).count();
    } else {
        s = usec = -1;
    }
}






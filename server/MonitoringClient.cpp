//
// Created by towa-ubuntu on 3/12/20.
//

#include "MonitoringClient.hpp"

/**Create an object for a client monitoring a specific file
 *
 * @param[in] address
 * @param[in] monitor_interval in ms
 */
MonitoringClient::MonitoringClient(sockaddr_storage address, int monitor_interval) : address(address) {
    end = std::chrono::steady_clock::now() + std::chrono::milliseconds{monitor_interval};
}

const sockaddr_storage &MonitoringClient::getAddress() const {
    return address;
}

bool MonitoringClient::isValid() const {
    return std::chrono::steady_clock::now() < end; //only valid until C++20, afterwards need to use <=>
}

bool MonitoringClient::expired() const {
    return !isValid();
}

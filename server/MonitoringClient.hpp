//
// Created by towa-ubuntu on 3/12/20.
//

#ifndef SERVER_MONITORINGCLIENT_HPP
#define SERVER_MONITORINGCLIENT_HPP

#include <chrono>
#include <netinet/in.h>

class MonitoringClient {
private:
    std::chrono::time_point<std::chrono::steady_clock> end;
    sockaddr_storage address;
public:
    MonitoringClient(sockaddr_storage address, int monitor_interval);

    sockaddr_storage getAddress() const; //TODO might return const sockaddr_storage&

    bool isValid() const;

    bool expired() const;
};


#endif //SERVER_MONITORINGCLIENT_HPP

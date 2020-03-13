//
// Created by towa-ubuntu on 3/12/20.
//

#ifndef SERVER_HANDLER_HPP
#define SERVER_HANDLER_HPP

#include "constants.hpp"
#include "UdpServer_linux.hpp"
#include "MonitoringClient.hpp"
#include <type_traits>
#include <unordered_map>
#include <vector>
#include <string>

using constants::Service_type;

class Handler {
public:
    void service(Service_type service_type, const UdpServer_linux &server, unsigned char *message);

private:
    std::unordered_map<std::string, std::vector<MonitoringClient>> registered_clients{};

    // overload function by the type integral_constant, each enum matches a type
    // this ensures function names can all be the same
    // advantage: services can have arbitrary names, only dependent on enum
    void
    exec_service(std::integral_constant<Service_type, Service_type::not_a_service>, const UdpServer_linux &server,
                 unsigned char *message);

    void exec_service(std::integral_constant<Service_type, Service_type::service1>, const UdpServer_linux &server,
                      unsigned char *message);

    void
    exec_service(std::integral_constant<Service_type, Service_type::register_client>, const UdpServer_linux &server,
                 unsigned char *message);

    void notify_registered_clients(const std::string &filename, const UdpServer_linux &server);
};


#endif //SERVER_HANDLER_HPP

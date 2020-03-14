//
// Created by towa-ubuntu on 3/12/20.
//

#include "Handler.hpp"
#include "utils.hpp"
#include <iostream>
#include <stdexcept>

using constants::Service_type;

/** Calls the function execution the service associated with the service type
 *
 * @param[in] service_type enum specifying which service to execute
 */
void Handler::service(Service_type service_type, const UdpServer_linux &server, unsigned char *message) {
    switch (service_type) {
        case Service_type::service1:
            exec_service(std::integral_constant<Service_type, Service_type::service1>{}, server, message);
            break;
        case Service_type::not_a_service:
            exec_service(std::integral_constant<Service_type, Service_type::not_a_service>{}, server, message);
            break;
        case Service_type::register_client:
            exec_service(std::integral_constant<Service_type, Service_type::register_client>{}, server, message);
            break;
        default:
            fprintf(stderr, "Service could not be identified!");
    }
}

void Handler::exec_service(std::integral_constant<constants::Service_type, constants::Service_type::not_a_service>,
                           const UdpServer_linux &server, unsigned char *message) {
    fprintf(stderr, "Error in system: Tried to execute the \"not_a_service\" service!");
}

void Handler::exec_service(std::integral_constant<constants::Service_type, constants::Service_type::service1>,
                           const UdpServer_linux &server, unsigned char *message) {
    std::cout << "Service 1 executed" << std::endl;
}

void
Handler::exec_service(std::integral_constant<Service_type, Service_type::register_client>, const UdpServer_linux &server,
                      unsigned char *message) {
    //TODO extract actual path/filename and monitoring length from message
    std::string filename{"Test"};
    int mon_interval = 100;
    registered_clients[filename].push_back(MonitoringClient{server.get_client_address(), mon_interval});
}

void Handler::notify_registered_clients(const std::string &filename, const UdpServer_linux &server) {
    std::vector<MonitoringClient> file_reg_clients;
    try {file_reg_clients = registered_clients.at(filename);}
    catch (const std::out_of_range &oor) {} // there is no entry for this filename => nothing to doo

    for (auto it = file_reg_clients.begin(); it != file_reg_clients.end(); ++it) {
        if(it->expired()) file_reg_clients.erase(it);
        else{
            //TODO send notification message to client
            fprintf(stdout, "Send notification to client");
        }
    }

}

void
Handler::store_message(const UdpServer_linux &server, const int requestID, const BytePtr message, const size_t len) {
    std::string clientName = utils::get_in_addr_str(&server.get_client_address());
    int port = utils::get_in_port(&server.get_client_address());
    stored_messages[clientName][port][requestID] = MessagePair(message, len);
}

bool Handler::is_duplicate_request(const sockaddr_storage *client_address, const int requestID) {
    std::string clientName = utils::get_in_addr_str(client_address);
    if (0 == stored_messages.count(clientName)) return false;

    int port = utils::get_in_port(client_address);
    if (0 == stored_messages[clientName].count(port)) return false;

    return !(0 == stored_messages[clientName][port].count(requestID));
}

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
#include <memory>

using constants::Service_type;
using std::unordered_map;

typedef std::shared_ptr<unsigned char[]> BytePtr;
typedef std::pair<BytePtr, size_t> MessagePair;

class Handler {
public:
    void service(Service_type service_type, const UdpServer_linux &server, unsigned char *message);

private:
    unordered_map<std::string, std::vector<MonitoringClient>> registered_clients{};

    /** field to store messages, if need to retrieve later
     *
     * first map for inet address; second map for port number; third map for requestID
     * final value is the pair of message (ptr) + len
     */
    unordered_map<std::string, unordered_map<int, unordered_map<int, MessagePair>>> stored_messages{};

    /**
     * overload function by the type integral_constant, each enum matches a type
     * this ensures function names can all be the same
     * advantage: services can have arbitrary names, only dependent on enum
     *
     * @param server
     * @param message
     */
    void
    exec_service(std::integral_constant<Service_type, Service_type::not_a_service>, const UdpServer_linux &server,
                 unsigned char *message);

    void exec_service(std::integral_constant<Service_type, Service_type::service1>, const UdpServer_linux &server,
                      unsigned char *message);

    void
    exec_service(std::integral_constant<Service_type, Service_type::register_client>, const UdpServer_linux &server,
                 unsigned char *message);

    void store_message(const UdpServer_linux &server, int requestID, BytePtr message, size_t len);

    void notify_registered_clients(const std::string &filename, const UdpServer_linux &server);

    bool is_duplicate_request(const sockaddr_storage *client_address, int requestID);
};


#endif //SERVER_HANDLER_HPP

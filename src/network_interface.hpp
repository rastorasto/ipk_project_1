#ifndef NETWORK_INTERFACE_HPP
#define NETWORK_INTERFACE_HPP

#include <set>
#include <string>

class NetworkInterface {
public:
    static void check_interfaces(std::set<std::string> &interfaces);
    static void list_interfaces(const std::set<std::string>& interfaces);
};

#endif // NETWORK_INTERFACE_HPP

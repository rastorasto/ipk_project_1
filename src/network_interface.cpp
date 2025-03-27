#include "network_interface.hpp"
#include <iostream>
#include <ifaddrs.h>
#include <net/if.h>
#include <cstdlib>

// Lists all interfaces
void NetworkInterface::check_interfaces(std::set<std::string> &interfaces) {
    struct ifaddrs *addrs, *tmp;
    getifaddrs(&addrs);
    for (tmp = addrs; tmp != NULL; tmp = tmp->ifa_next) {
        if (tmp->ifa_addr && // Interface has address
            (tmp->ifa_flags & IFF_UP) && // Interface is up
            (tmp->ifa_addr->sa_family == AF_INET || tmp->ifa_addr->sa_family == AF_INET6)) { // Interface is IPv4 or IPv6
            interfaces.insert(tmp->ifa_name);
        }
    }
    freeifaddrs(addrs);
}

void NetworkInterface::list_interfaces(const std::set<std::string>& interfaces) {
    // Prints the interfaces
    for(const auto& interface : interfaces) {
        std::cout << interface << std::endl;
    }
}

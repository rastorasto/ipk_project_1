#include "scanner_config.hpp"
#include "network_interface.hpp"
#include <netdb.h>
#include <arpa/inet.h>
#include <stdexcept>
#include <cstdlib>

using namespace std;

// Resolve target to IP address
void ScannerConfig::resolve_target() {
    struct addrinfo hints = {}, *res;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(target.c_str(), nullptr, &hints, &res) != 0) {
        throw invalid_argument("Invalid target");
    }

    // Go through all addresses and add them to the list
    for (auto *p = res; p != nullptr; p = p->ai_next) {
        char ipv4[INET_ADDRSTRLEN];
        char ipv6[INET6_ADDRSTRLEN];
        if (p->ai_family == AF_INET) {
            inet_ntop(AF_INET, &reinterpret_cast<sockaddr_in*>(p->ai_addr)->sin_addr, ipv4, sizeof(ipv4));
            ipv4_targets.push_back(ipv4);
        } else {
            inet_ntop(AF_INET6, &reinterpret_cast<sockaddr_in6*>(p->ai_addr)->sin6_addr, ipv6, sizeof(ipv6));
            ipv6_targets.push_back(ipv6);
        }
    }
    freeaddrinfo(res);

    if (ipv4_targets.empty() && ipv6_targets.empty()) {
        throw invalid_argument("No IP address found for target");
    }
}

// Validate that all required arguments are set
void ScannerConfig::validate() {
    NetworkInterface::check_interfaces(interfaces);

    // If no arguments are set, list all interfaces
    if (interface.empty() && tcp_ports.empty() && udp_ports.empty() && !timeout_set) {
        NetworkInterface::list_interfaces(interfaces);
        exit(0);
    }

    // Check if all required arguments are set
    if (interface.empty() || target.empty() || (udp_ports.empty() && tcp_ports.empty())) { // At least one port either tcp or udp must be set
        throw invalid_argument("Missing arguments");
    }

    // Check if the interface is valid
    for (const auto& iface : interfaces) {
        if (iface == interface) {
            resolve_target();// If the interface is valid, resolve the target
            return;
        }
    }
    throw invalid_argument("Invalid interface");
}

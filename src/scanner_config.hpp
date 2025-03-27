#ifndef SCANNER_CONFIG_HPP
#define SCANNER_CONFIG_HPP

#include <string>
#include <set>
#include <vector>

class ScannerConfig {
public:
    std::string interface;
    std::set<int> tcp_ports;
    std::set<int> udp_ports;
    int timeout = 5000;
    bool timeout_set = false;
    std::string target;
    std::vector<std::string> ipv4_targets;
    std::vector<std::string> ipv6_targets;
    std::set<std::string> interfaces;

    void resolve_target();
    void validate();
};

#endif // SCANNER_CONFIG_HPP

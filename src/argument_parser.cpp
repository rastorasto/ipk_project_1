
#include "argument_parser.hpp"
#include "port_parser.hpp"
#include <iostream>
#include <getopt.h>
#include <stdexcept>

using namespace std;

// Print usage instructions
void ArgumentParser::help() {
    cout << "./ipk-l4-scan [-i interface | --interface interface] [--pu port-ranges | --pt port-ranges | -u port-ranges | -t port-ranges] {-w timeout} [hostname | ip-address]\n";
    cout << "-h/--help writes usage instructions to stdout and terminates\n";
    cout << "-i eth0 (just one interface to scan through) or --interface. If this parameter is not specified (and any other parameters as well), or if only -i/--interface is specified without a value (and any other parameters are unspecified), a list of active interfaces is printed (additional information beyond the interface list is welcome but not required).\n";
    cout << "-t or --pt, -u or --pu port-ranges - scanned tcp/udp ports, allowed entry e.g., --pt 22 or --pu 1-65535 or --pt 22,23,24. The --pu and --pt arguments can be specified separately, i.e. they do not have to occur both at once if the user wants only TCP or only UDP scanning\n";
    cout << "-w 3000 or --wait 3000, is the timeout in milliseconds to wait for a response for a single port scan. This parameter is optional, in its absence the value 5000 (i.e., five seconds) is used.\n";
    cout << "either hostname, or ip-address, which either is hostname (e.g., merlin.fit.vutbr.cz) or IPv4/IPv6 address of scanned device.\n";
    cout << "All arguments can be in any order.\n" << endl;
}

ScannerConfig ArgumentParser::parse_arguments(int argc, char *argv[]) {
    ScannerConfig config;
    // Getopt argument options
    const struct option long_options[] = {
        {"help",      no_argument,       nullptr, 'h'},
        {"interface", optional_argument, nullptr, 'i'},
        {"pt",        required_argument, nullptr, 't'},
        {"pu",        required_argument, nullptr, 'u'},
        {"wait",      required_argument, nullptr, 'w'},
        {nullptr,     0,                 nullptr, 0}
    };

    // Argument parsing
    int opt;
    while ((opt = getopt_long(argc, argv, "hi::t:u:w:", long_options, nullptr)) != -1) {
        vector<int> new_ports;
        switch (opt) {
            case 'h':
                help();
                exit(0);
            case 'i':
                if (optind < argc && argv[optind][0] != '-') {
                    config.interface = argv[optind];
                    optind++;
                } else {
                    config.interface="";
                }
                break;
            case 't':
            // Parse ports and add them to the set
                new_ports = PortParser::parse_ports(optarg);
                for (auto port : new_ports) {
                    config.tcp_ports.insert(port);
                }
                break;
            case 'u':
            // Parse ports and add them to the set
                new_ports = PortParser::parse_ports(optarg);
                for (auto port : new_ports) {
                    config.udp_ports.insert(port);
                }
                break;
            case 'w':
                config.timeout = stoi(optarg);
                config.timeout_set = true;
                break;
            default:
                throw invalid_argument("Invalid argument");
        }
    }

    // Sets the target
    if (optind < argc) {
        config.target = argv[optind];
    }

    // Validates the arguments before returning
    config.validate();

    return config;
}

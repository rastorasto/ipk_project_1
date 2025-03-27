#include "udp_scanner.hpp"
#include "scanner_config.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>
#include <cerrno>
#include <stdexcept>
#include <iostream>

using namespace std;

UDPScanner::SocketConfig::SocketConfig(const ScannerConfig& config) {
    // Setup UDP socket
    udp_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_sock < 0) throw runtime_error("UDP socket creation failed");

    // Bind to interface
    if (setsockopt(udp_sock, SOL_SOCKET, SO_BINDTODEVICE,
                   config.interface.c_str(), config.interface.size() + 1) < 0) {
        close(udp_sock);
        throw runtime_error("UDP interface bind failed");
    }

    // Setup raw socket for ICMP
    raw_sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (raw_sock < 0) {
        close(udp_sock);
        throw runtime_error("Raw socket creation failed check if executing the program with sudo");
    }

    // Bind raw socket to interface
    if (setsockopt(raw_sock, SOL_SOCKET, SO_BINDTODEVICE,
                   config.interface.c_str(), config.interface.size() + 1) < 0) {
        close(udp_sock);
        close(raw_sock);
        throw runtime_error("Raw interface bind failed");
    }

    // Set timeout for raw socket
    struct timeval tv {
        .tv_sec = config.timeout / 1000,
        .tv_usec = (config.timeout % 1000) * 1000
    };
    setsockopt(raw_sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
}

UDPScanner::SocketConfig::~SocketConfig() {
    close(udp_sock);
    close(raw_sock);
}

// Goes through all the targets and specified ports and scans them
// Sends a UDP packet to the target and waits for an ICMP response
vector<UDPScanner::result> UDPScanner::scan(const ScannerConfig& config) {
    vector<result> results;
    SocketConfig sockets(config);

    for (const auto& addr : config.ipv4_targets) {
        for (auto port : config.udp_ports) {
            send_UDP_packet(port, sockets.udp_sock, addr);
            bool status = check_icmp_response(sockets.raw_sock, addr);
            results.push_back({addr, port, status});
        }
    }

    return results;
}

// Prints the results
void UDPScanner::print_results(const vector<result>& results) {
    for (const auto& res : results) {
        cout << res.ip << " " << res.port << " udp " << (res.open ? "open" : "closed") << endl;
    }
}

// Sends an empty UDP packet to the target
void UDPScanner::send_UDP_packet(int port, int sock, const string& target) {
    // Destination address information
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, target.c_str(), &addr.sin_addr);

    // Send empty UDP packet
    sendto(sock, nullptr, 0, 0, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
}

// Checks the target response
bool UDPScanner::check_icmp_response(int raw_sock, const string& target) {
    char buffer[1024];
    sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    int bytes = recvfrom(raw_sock, buffer, sizeof(buffer), 0, reinterpret_cast<sockaddr*>(&addr), &addr_len);

    // Did not receive any data, therefore the port is open
    if (bytes < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return true;
        }
        return true; // Assume open on other errors
    }

    // Received data

    // Check if the data are from the target
    string source_ip = inet_ntoa(addr.sin_addr);
    if (source_ip != target) {
        // Not from the target so port is open
        return true;
    }

    // Structs for IP header
    struct iphdr *ip_header = reinterpret_cast<struct iphdr*>(buffer);
    int ip_header_len = ip_header->ihl * 4;
    // Structs for ICMP header
    struct icmphdr *icmp_header = reinterpret_cast<struct icmphdr*>(buffer + ip_header_len);

    // Check if the ICMP packet is a destination unreachable
    if (icmp_header->type == DEST_UNREACHABLE && icmp_header->code == DEST_UNREACHABLE) {
        return false;
    }

    // Received ICMP packet but not a destination unreachable
    return true;
}

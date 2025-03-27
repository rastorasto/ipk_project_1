#ifndef UDP_SCANNER_HPP
#define UDP_SCANNER_HPP

#include "scanner_config.hpp"
#include <vector>
#include <string>

class UDPScanner {
public:
    struct result {
        std::string ip;
        int port;
        bool open;
    };

    static constexpr int DEST_UNREACHABLE = 3;

    static std::vector<result> scan(const ScannerConfig& config);
    static void print_results(const std::vector<result>& results);

private:
    class SocketConfig {
    public:
        int udp_sock;
        int raw_sock;

        SocketConfig(const ScannerConfig& config);
        ~SocketConfig();
    };

    static void send_UDP_packet(int port, int sock, const std::string& target);
    static bool check_icmp_response(int raw_sock, const std::string& target);
};

#endif // UDP_SCANNER_HPP

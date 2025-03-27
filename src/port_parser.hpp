#ifndef PORT_PARSER_HPP
#define PORT_PARSER_HPP

#include <vector>
#include <string>

class PortParser {
public:
    static std::vector<int> parse_ports(const std::string& ports);
private:
    static bool validate_port(int port);
    static std::vector<int> parse_semicolon_ports(const std::string& ports);
    static std::vector<int> parse_range_ports(const std::string& ports, size_t dashPos);
};

#endif // PORT_PARSER_HPP

#include "port_parser.hpp"
#include <sstream>
#include <stdexcept>

using namespace std;

// Parse ports from string
vector<int> PortParser::parse_ports(const string& ports) {
    vector<int> result;
    istringstream tokenStream(ports);
    string token;
    size_t dash_pos;

    // Split by comma and go through each port
    while(getline(tokenStream, token, ',')) {
        // Port is a range of ports
        if ((dash_pos = token.find('-')) != string::npos) {
            vector<int> range = parse_range_ports(token, dash_pos);
            result.insert(result.end(), range.begin(), range.end());
        } else {
            // Ports are devided by comma
            vector<int> single = parse_semicolon_ports(token);
            result.insert(result.end(), single.begin(), single.end());
        }
    }
    return result;
}

// Check if the port number is within the allowed range
bool PortParser::validate_port(int port) {
    return port >= 1 && port <= 65535;
}

// Parse ports devided by comma
vector<int> PortParser::parse_semicolon_ports(const string& ports) {
    vector<int> result;
    istringstream tokenStream(ports);
    string portStr;
    // Go through each port devided by comma
    while(getline(tokenStream, portStr, ',')) {
        int port = stoi(portStr);
        if (validate_port(port)) { // Check port number
            result.push_back(port);
        } else {
            throw invalid_argument("Invalid port number");
        }
    }
    return result;
}

// Parse range of ports
vector<int> PortParser::parse_range_ports(const string& ports, size_t dashPos) {
    // Get start and end of the range
    int start = stoi(ports.substr(0, dashPos));
    int end = stoi(ports.substr(dashPos + 1));

    if (!validate_port(start) || !validate_port(end)) {
        throw invalid_argument("Invalid port number");
    }

    if (start > end) {
        throw invalid_argument("Start port must be less than or equal to end port");
    }

    // Create a vector of ports in the range
    vector<int> result;
    for (int port = start; port <= end; ++port) {
        result.push_back(port);
    }
    return result;
}

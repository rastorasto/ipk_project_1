#include <iostream>
#include <string>
#include <pcap.h>
#include <getopt.h>
#include <vector>
#include <sstream>
#include <ifaddrs.h>
#include <net/if.h>
#include <set>


using namespace std;

// Print usage instructions
void help(){
  cout << "./ipk-l4-scan [-i interface | --interface interface] [--pu port-ranges | --pt port-ranges | -u port-ranges | -t port-ranges] {-w timeout} [hostname | ip-address]"; 
  
  cout << "-h/--help writes usage instructions to stdout and terminates";
  cout << "-i eth0 (just one interface to scan through) or --interface. If this parameter is not specified (and any other parameters as well), or if only -i/--interface is specified without a value (and any other parameters are unspecified), a list of active interfaces is printed (additional information beyond the interface list is welcome but not required).";
  cout << "-t or --pt, -u or --pu port-ranges - scanned tcp/udp ports, allowed entry e.g., --pt 22 or --pu 1-65535 or --pt 22,23,24. The --pu and --pt arguments can be specified separately, i.e. they do not have to occur both at once if the user wants only TCP or only UDP scanning";
  cout << "-w 3000 or --wait 3000, is the timeout in milliseconds to wait for a response for a single port scan. This parameter is optional, in its absence the value 5000 (i.e., five seconds) is used.";
  cout << "either hostname, or ip-address, which either is hostname (e.g., merlin.fit.vutbr.cz) or IPv4/IPv6 address of scanned device.";
  cout << "All arguments can be in any order.";
}

// Check if the port number is within the allowed range
bool validate_port(int port){
  return port >= 1 && port <= 65535;
}

// Parse ports devided by comma
vector<int> parse_semicolon_ports(string ports){
  vector<int> result;
  istringstream tokenStream(ports);
  // Go through each port devided by comma
  while(getline(tokenStream, ports, ',')){
    if(validate_port(stoi(ports))){ // Checks port number
      result.push_back(stoi(ports));
    } else {
      throw invalid_argument("Invalid port number");
    }
  }
  return result;
}

// Parse range of ports
vector<int> parse_range_ports(string ports, size_t dashPos) {
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

// Parse ports from string
vector<int> parse_ports(string ports) {
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


// Prints arguments for debugging purposes
void print_args(string interface, vector<int> tcp_ports, vector<int> udp_ports, int timeout, string target){
  cout << "Interface: " << interface << endl;
  cout << "TCP ports: ";
  for(uint i = 0; i < tcp_ports.size(); i++){
    cout << tcp_ports[i] << " ";
  }
  cout << endl;
  cout << "UDP ports: ";
  for (uint i = 0; i < udp_ports.size(); i++){
    cout << udp_ports[i] << " ";
  }
  cout << endl;
  cout << "Timeout: " << timeout << endl;
  cout << "Target: " << target << endl;
}

// Lists all interfaces
void list_interfaces(){
    struct ifaddrs *ifaddr, *ifa;
    set<string> interfaces; // I got duplicates so i used set to remove them
    
    // Get all interfaces
    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        exit(EXIT_FAILURE);
    }
    
    // Go through all interfaces and add them to the set
    for (ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr != nullptr){
          interfaces.insert(ifa->ifa_name);
        }
    }

    freeifaddrs(ifaddr);

    // Print all interfaces
    cout << "Active network interfaces:" << endl;
    for (const string &interface : interfaces) {
        cout << interface << endl;
    }

}

int main(int argc, char *argv[]){
  string interface;
  vector<int> tcp_ports;
  vector<int> udp_ports;
  int timeout = 5000;
  bool timeout_set = false;
  string target;

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
              return 0;
          case 'i':
              // cout << "optarg: " << optarg << endl;
              // interface = optarg ? optarg : "";
              // break;
              if (optarg) {
                  interface = optarg;
              } else if (optind < argc && argv[optind][0] != '-') {
                  interface = argv[optind];
                  optind++;
              } else {
                  interface = "";  // todo delete
              }
            break;
          case 't':
              new_ports = parse_ports(optarg);
              tcp_ports.insert(tcp_ports.end(), new_ports.begin(), new_ports.end());
              //tcp_ports.push_back(stoi(optarg));
              break;
          case 'u':
              new_ports = parse_ports(optarg);
              udp_ports.insert(udp_ports.end(), new_ports.begin(), new_ports.end());
              //udp_ports.push_back(stoi(optarg));
              break;
          case 'w':
              timeout = stoi(optarg);
              timeout_set = true;
              break;
          default:
              return 1;
      }
  }


  if(interface.empty() && tcp_ports.empty() && udp_ports.empty() && timeout_set == false){
    list_interfaces();
    return 0;
  }

  if(optind < argc){
    target = argv[optind];
  } else {
    std::cerr << "Error: Missing target argument.\n";
    return 1;
  }

  
  print_args(interface, tcp_ports, udp_ports, timeout, target);

}

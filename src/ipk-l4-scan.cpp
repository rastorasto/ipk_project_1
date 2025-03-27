#include "argument_parser.hpp"
#include "udp_scanner.hpp"
#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {
    try {
        ScannerConfig config = ArgumentParser::parse_arguments(argc, argv);// Parse arguments and set the configuration
        vector<UDPScanner::result> results = UDPScanner::scan(config);// Scan the target for ports
        UDPScanner::print_results(results);// Print the results
    } catch (const invalid_argument &e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    return 0;
}

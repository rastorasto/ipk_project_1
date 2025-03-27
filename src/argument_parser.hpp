#ifndef ARGUMENT_PARSER_HPP
#define ARGUMENT_PARSER_HPP

#include "scanner_config.hpp"

class ArgumentParser {
public:
    static void help();
    static ScannerConfig parse_arguments(int argc, char *argv[]);
};

#endif // ARGUMENT_PARSER_HPP

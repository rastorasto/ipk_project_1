# Changelog

## Implemented Functionality
- **UDP IPv4 Scanning**: Detects open/closed ports
- **CLI Arguments**: Parses command line arguments into a structure that is validated and if some arguments are missing or invalid, gives an error message.
- **Port arguments parsing**: Works with ports devided by comma as well as hyphen for example 10,12,15-20,23 and so on. Arguments can be repeated and they are stored in a set therefore there are no duplicates.
- **DNS Lookup**: Resolves domain names to IP addresses that are added into lists in structure.

## Known Issues
- **UDP IPv6 Scanning**: Not implemented
- **TCP IPv4/IPv6 Scanning**: Not implemented

# IPK Layer 4 Scanner (UDP Port Scanner)

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

## Table of Contents
1. [Executive Summary](#executive-summary)
2. [Theory of Operation](#theory-of-operation)
3. [Implementation Details](#implementation-details)
4. [Testing Methodology](#testing-methodology)
5. [Bibliography](#bibliography)

---

## Executive Summary
I implemented a UDP port scanner that scans ports using UDP packets and ICMP error messages. It resolves hostnames into ipv4 and ipv6 addresses but works only with ipv4.
- Interface selection (`-i eth0`, `-i wlan0`)
- Custom port ranges (e.g., `80`, `1-150`, `21,22,443`)
- Configurable timeout (`-w 400` milliseconds)
- Hostname/IPv4/IPv6 target resolution

---

## Theory of Operation
### UDP Scanning Principle
1. **Send**: I send an empty UDP packet to the target port.
2. **Listen**: Waiting for ICMP packet "Destination Unreachable".
3. **Closed Port**: Gets ICMP packet "Destination Unreachable" and verifies it's from the target IP.
4. **Open/Filtered Port**: If no response comes, the port is considered open.

---

### Key Protocols
- **UDP**: Used for sending packets.
- **ICMP**: Used for checking port state.
- **ARP**: Interface detection via `getifaddrs`.

---

## Implementation Details
### Code Structure
- src/
- argument_parser.[cpp|hpp]    # CLI argument parsing
- network_interface.[cpp|hpp]  # Interface listing/validation
- port_parser.[cpp|hpp]        # Port range parsing
- scanner_config.[cpp|hpp]     # Stores configuration
- udp_scanner.[cpp|hpp]        # UDP packet sending and ICMP response handling

---

## Testing Methodology
### Test environment
| Component       | Version/Specification                                  |
|-----------------|-------------------------------------------------------|
| OS              | Ubuntu (kernel: `5.15.0-134-generic`)                 |
| Architecture    | `aarch64` (ARM 64-bit)                                |
| Compiler        |  GCC 11.3.0               |
| Tools           | Wireshark, Netcat |

### Test Cases

1. Closed port detection

What:

    Verify ICMP response for closed ports.

How:

    Run scanner: ./ipk-l4-scan -i lo -u 9999 127.0.0.1

    Observed ICMP "Destination Unreachable" in Wireshark.

Result:

    127.0.0.1 9999 udp closed

2. Open port detection

What:

    Verify that timeout marks port as open.

How:

    Open udp port 80 using netcat: nc -ul 4444
    Run scanner: ./ipk-l4-scan -i lo -u 4444 127.0.0.1

    Observed no response in wireshark

Result:

    127.0.0.1 80 udp open

---

## Bibliography
1. **RFC 793** TRANSMISSION CONTROL PROTOCOL, Available from : https://tools.ietf.org/html/rfc793 [Accessed 27 March 2025]

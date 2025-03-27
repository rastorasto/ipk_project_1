CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -pedantic -I src -lpcap
SRC_DIR = src
BUILD_DIR = build

OBJS = $(BUILD_DIR)/ipk-l4-scan.o \
       $(BUILD_DIR)/network_interface.o \
       $(BUILD_DIR)/port_parser.o \
       $(BUILD_DIR)/scanner_config.o \
       $(BUILD_DIR)/argument_parser.o \
       $(BUILD_DIR)/udp_scanner.o

all: $(BUILD_DIR) ipk-l4-scan

ipk-l4-scan: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR) ipk-l4-scan

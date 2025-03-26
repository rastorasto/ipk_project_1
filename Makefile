# Author: Rastislav Uhliar (xuhliar00)

CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -Werror -lpcap
all: ipk-l4-scan

ipk-l4-scan: ipk-l4-scan.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f ipk-l4-scan

CXX = g++
CXXFLAGS = -Wall -O2

all: server client

server: server.cpp
	$(CXX) $(CXXFLAGS) server.cpp -o server

client: client.cpp
	$(CXX) $(CXXFLAGS) -fopenmp client.cpp -o client

clean:
	rm -f server client

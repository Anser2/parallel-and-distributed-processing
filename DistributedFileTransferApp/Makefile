# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

# Source files
SERVER_SRC = server.cpp
CLIENT_SRC = client.cpp

# Executables
SERVER_EXE = server
CLIENT_EXE = client

# Include directory (if network_utils.h is in a different folder, adjust accordingly)
INCLUDE_DIR = .

# Libraries (for networking, include -lws2_32 on Windows)
LIBS = 

# Detect OS and add platform-specific flags
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S), Linux)
    LIBS += -lpthread
endif
ifeq ($(UNAME_S), MINGW32_NT)
    LIBS += -lws2_32
endif

# Default target
all: $(SERVER_EXE) $(CLIENT_EXE)

# Compile server
$(SERVER_EXE): $(SERVER_SRC)
    $(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -o $@ $< $(LIBS)

# Compile client
$(CLIENT_EXE): $(CLIENT_SRC)
    $(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -o $@ $< $(LIBS)

# Clean up build files
clean:
    rm -f $(SERVER_EXE) $(CLIENT_EXE)

# Phony targets
.PHONY: all clean
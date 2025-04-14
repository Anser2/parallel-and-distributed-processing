#ifndef NETWORK_UTILS_H
#define NETWORK_UTILS_H

#ifdef _WIN32
    #define _WINSOCK_DEPRECATED_NO_WARNINGS // Optional: Suppress some warnings
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib") // Link with Winsock library automatically (MSVC)
    using SocketType = SOCKET;
    const SocketType INVALID_SOCKET_TYPE = INVALID_SOCKET;
#else // Linux, macOS, other POSIX
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h> // For close()
    #include <cerrno>   // For errno
    #include <cstring>  // For strerror
    using SocketType = int;
    const SocketType INVALID_SOCKET_TYPE = -1;
    #define SOCKET_ERROR -1
#endif

#include <string>
#include <stdexcept>
#include <iostream>

// Initialize networking (required for Winsock)
inline bool initializeNetworking() {
#ifdef _WIN32
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed: " << result << std::endl;
        return false;
    }
#endif
    return true;
}

// Cleanup networking (required for Winsock)
inline void cleanupNetworking() {
#ifdef _WIN32
    WSACleanup();
#endif
}

// Close socket (platform-specific)
inline int closeSocket(SocketType sock) {
#ifdef _WIN32
    return closesocket(sock);
#else
    return close(sock);
#endif
}

// Get last network error message
inline std::string getLastErrorMsg() {
#ifdef _WIN32
    int error_code = WSAGetLastError();
    char* msg_buf = nullptr;
    FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, error_code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&msg_buf, 0, nullptr);
    std::string msg = (msg_buf != nullptr) ? msg_buf : "Unknown Winsock error";
    LocalFree(msg_buf);
    return msg + " (Code: " + std::to_string(error_code) + ")";
#else
    return std::string(strerror(errno)) + " (Code: " + std::to_string(errno) + ")";
#endif
}

#endif // NETWORK_UTILS_H
// COMPILE USING : g++ -std=c++17 server.cpp -o server -lstdc++fs

#include "network_utils.h" // Include our helper header
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <filesystem> // Requires C++17
#include <stdexcept>
#include <sstream>
#include <cstdint> // For uint64_t

// --- Configuration ---
const int PORT = 65432;
const std::string FILE_DIR = "shared_files";
const int BUFFER_SIZE = 4096;
// --- End Configuration ---

namespace fs = std::filesystem;

// Function to get list of files in the directory
std::vector<std::string> getFileList(const std::string& directory) {
    std::vector<std::string> files;
    try {
        if (!fs::exists(directory) || !fs::is_directory(directory)) {
             std::cerr << "Error: Directory '" << directory << "' not found or is not a directory." << std::endl;
             // Create it if it doesn't exist
             if (!fs::exists(directory)) {
                std::cout << "Creating directory: " << directory << std::endl;
                fs::create_directory(directory);
                std::cerr << "Please add files to '" << directory << "' and restart server." << std::endl;
             }
             return files; // Return empty list
        }
        for (const auto& entry : fs::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                files.push_back(entry.path().filename().string());
            }
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error accessing '" << directory << "': " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error listing files: " << e.what() << std::endl;
    }
    return files;
}

// Function to handle a single client connection
void handleClient(SocketType clientSocket) {
    char buffer[BUFFER_SIZE];
    int bytesReceived;

    try {
        // 1. Get and send the file list
        std::vector<std::string> files = getFileList(FILE_DIR);
        std::stringstream listStream;
        for (const auto& filename : files) {
            listStream << filename << "\n"; // Newline separated list
        }
        std::string fileListStr = listStream.str();
        // Remove trailing newline if list is not empty
        if (!fileListStr.empty()) {
            fileListStr.pop_back();
        }

        // Send size of the list string first
        uint32_t listSize = static_cast<uint32_t>(fileListStr.size());
        uint32_t networkListSize = htonl(listSize); // Ensure network byte order
        if (send(clientSocket, reinterpret_cast<const char*>(&networkListSize), sizeof(networkListSize), 0) == SOCKET_ERROR) {
             throw std::runtime_error("Failed to send file list size: " + getLastErrorMsg());
        }

        // Send the actual list string
        if (!fileListStr.empty()) {
             if (send(clientSocket, fileListStr.c_str(), listSize, 0) == SOCKET_ERROR) {
                  throw std::runtime_error("Failed to send file list: " + getLastErrorMsg());
             }
        }
        std::cout << "Sent file list (" << listSize << " bytes)." << std::endl;


        // 2. Receive the requested filename
        bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);
        if (bytesReceived == SOCKET_ERROR) {
            throw std::runtime_error("Failed to receive filename: " + getLastErrorMsg());
        }
        if (bytesReceived == 0) {
             std::cout << "Client disconnected before sending filename." << std::endl;
             return;
        }
        buffer[bytesReceived] = '\0'; // Null-terminate
        std::string requestedFile(buffer);
        std::cout << "Client requested: " << requestedFile << std::endl;

        // 3. Check if file exists and send it
        fs::path filePath = fs::path(FILE_DIR) / requestedFile;
        bool fileExists = false;
        uint64_t fileSize = 0;

        // Check if requested file is in our list and actually exists
        bool foundInList = false;
        for(const auto& fname : files) {
            if (fname == requestedFile) {
                foundInList = true;
                break;
            }
        }

        if (foundInList && fs::exists(filePath) && fs::is_regular_file(filePath)) {
            try {
                fileSize = fs::file_size(filePath);
                fileExists = true;
            } catch (const fs::filesystem_error& e) {
                std::cerr << "Error getting file size for '" << filePath << "': " << e.what() << std::endl;
                // Proceed as if file doesn't exist for transfer purposes
            }
        }

        if (fileExists) {
            // Send OK response with file size
            std::string ok_msg = "OK:" + std::to_string(fileSize);
             if (send(clientSocket, ok_msg.c_str(), ok_msg.length(), 0) == SOCKET_ERROR) {
                throw std::runtime_error("Failed to send OK message: " + getLastErrorMsg());
             }

            // Optional: Wait for ACK from client (simple handshake)
            bytesReceived = recv(clientSocket, buffer, 3, 0); // Expect "ACK"
             if (bytesReceived <= 0 || std::string(buffer, bytesReceived) != "ACK") {
                 std::cerr << "Client did not ACK correctly. Aborting file send." << std::endl;
                 return;
             }

            std::cout << "Sending file '" << requestedFile << "' (" << fileSize << " bytes)..." << std::endl;
            std::ifstream fileStream(filePath, std::ios::binary);
            if (!fileStream.is_open()) {
                throw std::runtime_error("Failed to open file for reading: " + requestedFile);
            }

            uint64_t bytesSent = 0;
            while (bytesSent < fileSize) {
                fileStream.read(buffer, BUFFER_SIZE);
                std::streamsize bytesRead = fileStream.gcount();
                if (bytesRead <= 0) break; // Should not happen if size is correct, but safety

                int sent = send(clientSocket, buffer, static_cast<int>(bytesRead), 0);
                if (sent == SOCKET_ERROR) {
                     fileStream.close();
                     throw std::runtime_error("Error sending file chunk: " + getLastErrorMsg());
                }
                bytesSent += sent;
            }
            fileStream.close();

            if (bytesSent == fileSize) {
                std::cout << "Finished sending " << requestedFile << std::endl;
            } else {
                 std::cerr << "Error: Sent " << bytesSent << " bytes, but expected " << fileSize << std::endl;
            }

        } else {
            // Send Error response
            std::string error_msg = "ERROR:File '" + requestedFile + "' not found or unavailable.";
            std::cout << error_msg << std::endl;
             if (send(clientSocket, error_msg.c_str(), error_msg.length(), 0) == SOCKET_ERROR) {
                 // Log error, but client might already be gone
                 std::cerr << "Failed to send ERROR message: " << getLastErrorMsg() << std::endl;
             }
        }

    } catch (const std::exception& e) {
        std::cerr << "Error handling client: " << e.what() << std::endl;
    }

    // Close client socket
    closeSocket(clientSocket);
    std::cout << "Client connection closed." << std::endl;
}


int main() {
    if (!initializeNetworking()) {
        return 1;
    }

    SocketType serverSocket = INVALID_SOCKET_TYPE;
    SocketType clientSocket = INVALID_SOCKET_TYPE;
    sockaddr_in serverAddr{};
    sockaddr_in clientAddr{};
    #ifdef _WIN32
        int clientAddrSize = sizeof(clientAddr);
    #else
        socklen_t clientAddrSize = sizeof(clientAddr);
    #endif

    try {
        // Create socket
        serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (serverSocket == INVALID_SOCKET_TYPE) {
            throw std::runtime_error("Socket creation failed: " + getLastErrorMsg());
        }

        // Allow address reuse
        int opt = 1;
        #ifdef _WIN32
            setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
        #else
            setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        #endif


        // Bind socket
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY; // Listen on all available interfaces
        serverAddr.sin_port = htons(PORT);       // Convert port to network byte order

        if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
            throw std::runtime_error("Bind failed: " + getLastErrorMsg());
        }

        // Listen for connections
        if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) { // SOMAXCONN = backlog queue size
             throw std::runtime_error("Listen failed: " + getLastErrorMsg());
        }

        std::cout << "Server listening on port " << PORT << "..." << std::endl;
        std::cout << "Serving files from: " << fs::absolute(FILE_DIR).string() << std::endl;


        // Accept connections loop
        while (true) {
            clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);
            if (clientSocket == INVALID_SOCKET_TYPE) {
                std::cerr << "Accept failed: " << getLastErrorMsg() << std::endl;
                continue; // Continue listening for other connections
            }

            char clientIp[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &clientAddr.sin_addr, clientIp, INET_ADDRSTRLEN);
            std::cout << "\nConnection accepted from " << clientIp << ":" << ntohs(clientAddr.sin_port) << std::endl;

            // Handle the client in a separate function (could use threads here for concurrency)
            handleClient(clientSocket);
            // Note: handleClient closes the clientSocket
            clientSocket = INVALID_SOCKET_TYPE; // Reset for safety
        }

    } catch (const std::exception& e) {
        std::cerr << "Server runtime error: " << e.what() << std::endl;
    }

    // Cleanup
    if (clientSocket != INVALID_SOCKET_TYPE) {
        closeSocket(clientSocket);
    }
    if (serverSocket != INVALID_SOCKET_TYPE) {
        closeSocket(serverSocket);
    }
    cleanupNetworking();

    return 0;
}
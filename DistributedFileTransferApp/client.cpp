#include "network_utils.h" // Include our helper header
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <filesystem> // Requires C++17
#include <stdexcept>
#include <sstream>
#include <limits> // For numeric_limits
#include <cstdlib> // For system()
#include <cstdint> // For uint64_t

// --- Configuration ---
const std::string SERVER_HOST = "127.0.0.1"; // Server IP or hostname
const int SERVER_PORT = 65432;
const std::string DOWNLOAD_DIR = "downloads";
const int BUFFER_SIZE = 4096;
// --- End Configuration ---

namespace fs = std::filesystem;

// Function to attempt opening the file
void openFile(const std::string& filepath) {
    std::cout << "\nAttempting to open downloaded file..." << std::endl;
    std::string command;
#ifdef _WIN32
    // Use "start" command, needs quoted path if it contains spaces
    command = "start \"\" \"" + filepath + "\"";
#else
    command = "xdg-open \"" + filepath + "\""; // Linux
#endif

    std::cout << "Executing: " << command << std::endl;
    int result = system(command.c_str());

    if (result != 0) {
        std::cerr << "Warning: Command to open file might have failed (return code: " << result << ")." << std::endl;
         std::cerr << "The file was saved to: " << filepath << std::endl;
    } else {
        std::cout << "Check if the application opened successfully and the file content is correct." << std::endl;
    }
}


int main() {
    if (!initializeNetworking()) {
        return 1;
    }

    // Create download directory if it doesn't exist
    try {
        if (!fs::exists(DOWNLOAD_DIR)) {
            std::cout << "Creating directory: " << DOWNLOAD_DIR << std::endl;
            fs::create_directory(DOWNLOAD_DIR);
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Error creating download directory '" << DOWNLOAD_DIR << "': " << e.what() << std::endl;
        cleanupNetworking();
        return 1;
    }


    SocketType sock = INVALID_SOCKET_TYPE;
    sockaddr_in serverAddr{};
    char buffer[BUFFER_SIZE];

    try {
        // Create socket
        sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (sock == INVALID_SOCKET_TYPE) {
            throw std::runtime_error("Socket creation failed: " + getLastErrorMsg());
        }

        // Prepare server address structure
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(SERVER_PORT);
        // Convert IPv4 address from text to binary form
        #ifdef _WIN32
            if (inet_pton(AF_INET, SERVER_HOST.c_str(), &serverAddr.sin_addr) <= 0) {
                 throw std::runtime_error("Invalid address/ Address not supported: " + getLastErrorMsg());
            }
        #else
             if (inet_pton(AF_INET, SERVER_HOST.c_str(), &serverAddr.sin_addr) <= 0) {
                  throw std::runtime_error("Invalid address/ Address not supported");
             }
        #endif


        // Connect to server
        std::cout << "Connecting to server " << SERVER_HOST << ":" << SERVER_PORT << "..." << std::endl;
        if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
            throw std::runtime_error("Connection failed: " + getLastErrorMsg());
        }
        std::cout << "Connected!" << std::endl;

        // 1. Receive file list size
        uint32_t networkListSize;
        int bytesReceived = recv(sock, reinterpret_cast<char*>(&networkListSize), sizeof(networkListSize), 0);
        if (bytesReceived == SOCKET_ERROR) {
             throw std::runtime_error("Failed to receive file list size: " + getLastErrorMsg());
        }
         if (bytesReceived != sizeof(networkListSize)) {
              throw std::runtime_error("Received incomplete file list size.");
         }
        uint32_t listSize = ntohl(networkListSize); // Convert back from network byte order


        // 2. Receive file list string
        std::string fileListStr;
        if (listSize > 0) {
             fileListStr.resize(listSize); // Allocate space
             uint32_t totalReceived = 0;
             while(totalReceived < listSize) {
                  int receivedNow = recv(sock, &fileListStr[totalReceived], listSize - totalReceived, 0);
                  if (receivedNow == SOCKET_ERROR) {
                       throw std::runtime_error("Error receiving file list: " + getLastErrorMsg());
                  }
                   if (receivedNow == 0) { // Server disconnected prematurely
                       throw std::runtime_error("Server disconnected while sending file list.");
                   }
                  totalReceived += receivedNow;
             }
        } else {
             std::cout << "Server sent an empty file list." << std::endl;
        }


        // Parse the list
        std::vector<std::string> availableFiles;
        std::stringstream ss(fileListStr);
        std::string filename;
        while (std::getline(ss, filename, '\n')) {
            if (!filename.empty()) { // Avoid potential empty lines
                 availableFiles.push_back(filename);
            }
        }

        if (availableFiles.empty()) {
            std::cout << "No files available from the server." << std::endl;
            closeSocket(sock);
            cleanupNetworking();
            return 0;
        }

        // 3. Display list and get user choice
        std::cout << "\nAvailable files:" << std::endl;
        for (size_t i = 0; i < availableFiles.size(); ++i) {
            std::cout << i + 1 << ". " << availableFiles[i] << std::endl;
        }

        int choice = 0;
        while (true) {
            std::cout << "\nEnter the number of the file to download (1-" << availableFiles.size() << "), or 0 to cancel: ";
            std::cin >> choice;

            // Basic input validation
            if (std::cin.fail() || choice < 0 || choice > static_cast<int>(availableFiles.size())) {
                std::cout << "Invalid input. Please enter a number between 0 and " << availableFiles.size() << "." << std::endl;
                std::cin.clear(); // Clear error flags
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard bad input
            } else {
                 // Consume the newline character left by std::cin
                 std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                 break; // Valid input
            }
        }


        if (choice == 0) {
            std::cout << "Download cancelled." << std::endl;
            // No need to send anything specific, just close
        } else {
            std::string selectedFile = availableFiles[choice - 1];

            // 4. Send selected filename to server
            std::cout << "Requesting file: " << selectedFile << std::endl;
            if (send(sock, selectedFile.c_str(), selectedFile.length(), 0) == SOCKET_ERROR) {
                 throw std::runtime_error("Failed to send filename request: " + getLastErrorMsg());
            }

            // 5. Receive server response (OK:size or ERROR:...)
            bytesReceived = recv(sock, buffer, BUFFER_SIZE - 1, 0);
            if (bytesReceived == SOCKET_ERROR) {
                throw std::runtime_error("Failed to receive server response: " + getLastErrorMsg());
            }
             if (bytesReceived == 0) {
                 throw std::runtime_error("Server disconnected unexpectedly after filename request.");
             }
            buffer[bytesReceived] = '\0';
            std::string response(buffer);

            if (response.rfind("ERROR:", 0) == 0) { // Check if starts with "ERROR:"
                std::cerr << "Server Error: " << response.substr(6) << std::endl;
            } else if (response.rfind("OK:", 0) == 0) { // Check if starts with "OK:"
                uint64_t fileSize = 0;
                try {
                    fileSize = std::stoull(response.substr(3)); // Parse size after "OK:"
                    std::cout << "Server confirmed file. Expected size: " << fileSize << " bytes." << std::endl;
                } catch (const std::invalid_argument& e) {
                     throw std::runtime_error("Invalid file size received from server: " + response.substr(3));
                } catch (const std::out_of_range& e) {
                     throw std::runtime_error("File size received from server is out of range: " + response.substr(3));
                }


                // Send ACK back to server
                 if (send(sock, "ACK", 3, 0) == SOCKET_ERROR) {
                     throw std::runtime_error("Failed to send ACK to server: " + getLastErrorMsg());
                 }


                // 6. Receive file data
                fs::path downloadPath = fs::path(DOWNLOAD_DIR) / selectedFile;
                std::cout << "Downloading to: " << fs::absolute(downloadPath).string() << std::endl;

                std::ofstream outFile(downloadPath, std::ios::binary | std::ios::trunc);
                if (!outFile.is_open()) {
                    throw std::runtime_error("Failed to open file for writing: " + downloadPath.string());
                }

                uint64_t totalBytesReceived = 0;
                while (totalBytesReceived < fileSize) {
                    bytesReceived = recv(sock, buffer, std::min((int)BUFFER_SIZE, (int)(fileSize - totalBytesReceived)), 0);

                    if (bytesReceived == SOCKET_ERROR) {
                         outFile.close();
                         // Optionally delete partial file: fs::remove(downloadPath);
                         throw std::runtime_error("Error receiving file data: " + getLastErrorMsg());
                    }
                    if (bytesReceived == 0) {
                        outFile.close();
                        // Optionally delete partial file: fs::remove(downloadPath);
                        throw std::runtime_error("Server disconnected during file transfer.");
                    }

                    outFile.write(buffer, bytesReceived);
                    if (!outFile) { // Check for write errors
                         outFile.close();
                         throw std::runtime_error("Error writing file chunk to disk.");
                    }
                    totalBytesReceived += bytesReceived;

                    // Optional: Print progress
                    double progress = (fileSize == 0) ? 100.0 : (static_cast<double>(totalBytesReceived) / fileSize) * 100.0;
                     std::cout << "\rProgress: " << totalBytesReceived << "/" << fileSize << " bytes (" << std::fixed << std::setprecision(2) << progress << "%)" << std::flush;

                }
                 outFile.close(); // Close the file explicitly after loop finishes
                std::cout << std::endl; // New line after progress

                if (totalBytesReceived == fileSize) {
                    std::cout << "Download complete!" << std::endl;
                    // Try to open the file
                    openFile(fs::absolute(downloadPath).string());
                } else {
                    std::cerr << "Error: Download incomplete. Received " << totalBytesReceived << " of " << fileSize << " bytes." << std::endl;
                    // Optionally delete partial file
                     try { fs::remove(downloadPath); } catch(...) {}
                }

            } else {
                std::cerr << "Unknown response from server: " << response << std::endl;
            }
        } // end if (choice != 0)

    } catch (const std::exception& e) {
        std::cerr << "Client runtime error: " << e.what() << std::endl;
    }

    // Cleanup
    if (sock != INVALID_SOCKET_TYPE) {
        closeSocket(sock);
    }
    cleanupNetworking();

    return 0;
}
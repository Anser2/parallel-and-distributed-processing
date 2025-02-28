// The Server (Server.cpp)

#include <iostream>
#include <cstring>          // strlen()
#include <cstdlib>          // exit()
#include <arpa/inet.h>      // bind(), listen(), accept(), send(), recv()
#include <unistd.h>
#include <pthread.h>

#define MAXBUFFERSIZE 512   // Maximum buffer size
#define SERVERPORT 6000     // Default server port

// Server and client socket descriptors
int ServerSocketFD, ClientSocketFD;
sockaddr_in ServerAddress, ClientAddress;
socklen_t sin_size;

// Thread-safe buffer
pthread_mutex_t lock;

// Function to receive messages
void* recvfunc(void* arg) {
    char buffer[MAXBUFFERSIZE];

    while (true) {
        int bytesReceived = recv(ClientSocketFD, buffer, MAXBUFFERSIZE - 1, 0);
        if (bytesReceived <= 0) {
            std::cout << "Connection closed by client." << std::endl;
            break;
        }
        buffer[bytesReceived] = '\0';
        std::cout << "Client says: " << buffer << std::endl;

        if (strcmp(buffer, "bye") == 0) {
            std::cout << "Client said 'bye'. Closing connection." << std::endl;
            break;
        }
    }

    return nullptr;
}

// Function to send messages
void* sendfunc(void* arg) {
    char buffer[MAXBUFFERSIZE];

    while (true) {
        std::cout << "Enter message for the client: ";
        std::cin.getline(buffer, MAXBUFFERSIZE);  // Make sure input is handled correctly

        pthread_mutex_lock(&lock);
        if (send(ClientSocketFD, buffer, strlen(buffer), 0) == -1) {
            std::cerr << "Error sending message!" << std::endl;
        }
        pthread_mutex_unlock(&lock);

        if (strcmp(buffer, "bye") == 0) {
            std::cout << "Server exiting chat." << std::endl;
            break;
        }
    }

    return nullptr;
}


int main() {
    pthread_mutex_init(&lock, nullptr);

    ServerSocketFD = socket(AF_INET, SOCK_STREAM, 0);
    int Yes = 1;
    setsockopt(ServerSocketFD, SOL_SOCKET, SO_REUSEADDR, &Yes, sizeof(int));

    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_addr.s_addr = INADDR_ANY;
    ServerAddress.sin_port = htons(SERVERPORT);

    bind(ServerSocketFD, (sockaddr*)&ServerAddress, sizeof(ServerAddress));
    listen(ServerSocketFD, 5);

    std::cout << "Server listening on port " << SERVERPORT << std::endl;

    sin_size = sizeof(ClientAddress);
    ClientSocketFD = accept(ServerSocketFD, (sockaddr*)&ClientAddress, &sin_size);

    std::cout << "*** Connected to " << inet_ntoa(ClientAddress.sin_addr) << " ***" << std::endl;

    pthread_t sendThread, recvThread;
    pthread_create(&recvThread, nullptr, recvfunc, nullptr);
    pthread_create(&sendThread, nullptr, sendfunc, nullptr);

    pthread_join(recvThread, nullptr);
    pthread_join(sendThread, nullptr);

    close(ClientSocketFD);
    close(ServerSocketFD);
    pthread_mutex_destroy(&lock);

    return 0;
}

// The Client (Client.cpp)

#include <iostream>
#include <cstring>        // strlen()
#include <cstdlib>        // exit()
#include <netdb.h>        // gethostbyname(), connect(), send(), recv()
#include <unistd.h>
#include <pthread.h>

#define MAXBUFFERSIZE 512
#define SERVERPORT 6000
#define CLIENTPORT 6001

int ClientSocketFD;
struct hostent* he;
struct sockaddr_in ServerAddress, ClientAddress;
pthread_mutex_t lock;

// Function to send messages
void* sendfunc(void* arg) {
    char ClientMessage[MAXBUFFERSIZE];

    while (true) {
        std::cout << "Enter message for the server: ";
        std::cin.getline(ClientMessage, MAXBUFFERSIZE);

        pthread_mutex_lock(&lock);
        send(ClientSocketFD, ClientMessage, strlen(ClientMessage), 0);
        pthread_mutex_unlock(&lock);

        if (strcmp(ClientMessage, "bye") == 0) {
            std::cout << "Client exiting chat." << std::endl;
            break;
        }
    }

    return nullptr;
}

// Function to receive messages
void* recvfunc(void* arg) {
    char buffer[MAXBUFFERSIZE];

    while (true) {
        int bytesReceived = recv(ClientSocketFD, buffer, MAXBUFFERSIZE - 1, 0);
        if (bytesReceived <= 0) {
            std::cout << "Server disconnected." << std::endl;
            break;
        }

        buffer[bytesReceived] = '\0';
        std::cout << "Server says: " << buffer << std::endl;
    }

    return nullptr;
}

int main(int argc, char* argv[]) {
    pthread_mutex_init(&lock, nullptr);

    if (argc < 3) {
        std::cout << "Usage: ./client [server name or IP] [server port]" << std::endl;
        exit(-1);
    }

    he = gethostbyname(argv[1]);
    ClientSocketFD = socket(AF_INET, SOCK_STREAM, 0);

    int Yes = 1;
    setsockopt(ClientSocketFD, SOL_SOCKET, SO_REUSEADDR, &Yes, sizeof(int));

    // Initializing Server address
    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_addr = *((in_addr*)(*he).h_addr);
    ServerAddress.sin_port = htons(atoi(argv[2]));

    connect(ClientSocketFD, (sockaddr*)&ServerAddress, sizeof(ServerAddress));
    std::cout << "Connected to server!" << std::endl;

    // Create send/receive threads
    pthread_t sendThread, recvThread;
    pthread_create(&recvThread, nullptr, recvfunc, nullptr);
    pthread_create(&sendThread, nullptr, sendfunc, nullptr);

    pthread_join(recvThread, nullptr);
    pthread_join(sendThread, nullptr);

    close(ClientSocketFD);
    pthread_mutex_destroy(&lock);

    return 0;
}

// client_array_sum.cpp
#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
using namespace std;

ssize_t readAll(int fd, void* buffer, size_t count) {
    char* buf = static_cast<char*>(buffer);
    size_t totalRead = 0;
    while (totalRead < count) {
        ssize_t rc = read(fd, buf + totalRead, count - totalRead);
        if (rc <= 0) return rc;
        totalRead += rc;
    }
    return totalRead;
}

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serv_addr = {AF_INET, htons(8080)};
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);
    connect(sock, (sockaddr*)&serv_addr, sizeof(serv_addr));

    int count = 0;
    readAll(sock, &count, sizeof(int));

    vector<int> chunk(count);
    readAll(sock, chunk.data(), count * sizeof(int));

    int localSum = 0;
    for (int x : chunk) localSum += x;

    write(sock, &localSum, sizeof(int));
    cout << "[Client] Sent local sum: " << localSum << endl;
    close(sock);
    return 0;
}

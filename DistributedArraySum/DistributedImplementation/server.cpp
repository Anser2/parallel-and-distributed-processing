// server_array_sum.cpp
#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
using namespace std;

const int N = 100000;  // total elements

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
    vector<int> data(N);
    for (int i = 0; i < N; ++i) data[i] = 1; // example: all ones

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr = {AF_INET, htons(8080), INADDR_ANY};
    bind(server_fd, (sockaddr*)&addr, sizeof(addr));
    listen(server_fd, 2);
    cout << "[Server] Waiting for clients...\n";

    int client1 = accept(server_fd, nullptr, nullptr);
    int client2 = accept(server_fd, nullptr, nullptr);
    cout << "[Server] Both clients connected\n";

    // Divide data
    int half = N / 2;
    write(client1, &half, sizeof(int));
    write(client1, data.data(), half * sizeof(int));
    write(client2, &half, sizeof(int));
    write(client2, data.data() + half, half * sizeof(int));

    // Get partial sums
    int sum1 = 0, sum2 = 0;
    readAll(client1, &sum1, sizeof(int));
    readAll(client2, &sum2, sizeof(int));

    int total = sum1 + sum2;
    cout << "[Server] Final sum: " << total << endl;

    close(client1);
    close(client2);
    close(server_fd);
    return 0;
}

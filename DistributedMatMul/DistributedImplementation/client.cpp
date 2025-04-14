#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
using namespace std;

const int N = 4;

ssize_t readAll(int fd, void* buffer, size_t count) {
    char* buf = static_cast<char*>(buffer);
    size_t total = 0;
    while (total < count) {
        ssize_t r = read(fd, buf + total, count - total);
        if (r <= 0) return r;
        total += r;
    }
    return total;
}

int main() {
    vector<vector<int>> A(N, vector<int>(N));
    vector<vector<int>> B(N, vector<int>(N));
    vector<vector<int>> C(N / 2, vector<int>(N, 0));

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serv_addr = {AF_INET, htons(8080)};
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);
    connect(sock, (sockaddr*)&serv_addr, sizeof(serv_addr));

    // Receive A and B
    for (int i = 0; i < N; i++) readAll(sock, &A[i][0], N * sizeof(int));
    for (int i = 0; i < N; i++) readAll(sock, &B[i][0], N * sizeof(int));

    // Compute portion of C
    int start = 0, end = N / 2;
    if (fork() != 0) start = N / 2, end = N;  // fork for 2 clients

    for (int i = start; i < end; i++) {
        for (int j = 0; j < N; j++) {
            int sum = 0;
            for (int k = 0; k < N; k++) {
                sum += A[i][k] * B[k][j];
            }
            C[i - start][j] = sum;
        }
    }

    // Send result
    for (int i = 0; i < N / 2; i++)
        write(sock, &C[i][0], N * sizeof(int));

    close(sock);
    cout << "[Client " << start << "-" << end - 1 << "] Computation done.\n";
    return 0;
}

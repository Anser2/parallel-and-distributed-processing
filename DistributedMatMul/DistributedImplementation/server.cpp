#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iomanip>
using namespace std;

const int N = 4;  // Change as needed

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
    vector<vector<int>> A(N, vector<int>(N)), B(N, vector<int>(N)), C(N, vector<int>(N));

    // Fill A and B with sample data
    int val = 1;
    for (int i = 0; i < N; i++) 
        for (int j = 0; j < N; j++) 
            A[i][j] = val++, B[i][j] = 1; // Simple values for clarity

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr = {AF_INET, htons(8080), INADDR_ANY};
    bind(server_fd, (sockaddr*)&addr, sizeof(addr));
    listen(server_fd, 2);

    cout << "[Server] Waiting for clients...\n";
    int client1 = accept(server_fd, nullptr, nullptr);
    int client2 = accept(server_fd, nullptr, nullptr);
    cout << "[Server] Clients connected.\n";

    // Send A and B to both clients
    for (int i = 0; i < N; i++) {
        write(client1, &A[i][0], N * sizeof(int));
        write(client2, &A[i][0], N * sizeof(int));
    }
    for (int i = 0; i < N; i++) {
        write(client1, &B[i][0], N * sizeof(int));
        write(client2, &B[i][0], N * sizeof(int));
    }

    // Receive partial results
    for (int i = 0; i < N / 2; i++)
        readAll(client1, &C[i][0], N * sizeof(int));
    for (int i = N / 2; i < N; i++)
        readAll(client2, &C[i][0], N * sizeof(int));

    // Show result
    cout << "\n[Server] Final matrix C = A × B:\n";
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            cout << setw(4) << C[i][j];
        }
        cout << '\n';
    }

    close(client1);
    close(client2);
    close(server_fd);
    return 0;
}

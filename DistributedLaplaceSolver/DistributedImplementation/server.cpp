#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iomanip>
using namespace std;

const int N = 200;
const int MAX_ITER = 10000;  // Increased iterations for convergence

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
    vector<vector<double>> grid(N, vector<double>(N, 0.0));
    
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr = {AF_INET, htons(8080), INADDR_ANY};
    bind(server_fd, (sockaddr*)&addr, sizeof(addr));
    listen(server_fd, 2);

    cout << "[Server] Waiting for clients...\n";
    
    int client1 = accept(server_fd, nullptr, nullptr);
    int client2 = accept(server_fd, nullptr, nullptr);
    cout << "[Server] Both clients connected\n";

    // Boundary exchange loop
    for (int it = 0; it < MAX_ITER; it++) {
        double boundaries[2][N];
        
        // Receive boundaries from clients
        readAll(client1, boundaries[0], N*sizeof(double));
        readAll(client2, boundaries[1], N*sizeof(double));
        
        // Send boundaries to opposite clients
        write(client1, boundaries[1], N*sizeof(double));
        write(client2, boundaries[0], N*sizeof(double));
    }

    // Collect final data
    for (int i = 0; i < 100; i++) 
        readAll(client1, &grid[i][0], N*sizeof(double));
    
    for (int i = 100; i < N; i++) 
        readAll(client2, &grid[i][0], N*sizeof(double));

    // Diagnostic output
    cout << "[Server] Validation:\n";
    cout << "Top boundary: " << grid[0][100] << " (should be 5.0)\n";
    cout << "Bottom boundary: " << grid[199][100] << " (should be -5.0)\n";
    cout << "Center value: " << fixed << setprecision(5) << grid[100][100] << "\n";

    FILE* out = fopen("grid.bin", "wb");
    for (auto& row : grid) {
        fwrite(row.data(), sizeof(double), N, out);
    }
    fclose(out);

    close(client1);
    close(client2);
    close(server_fd);
    return 0;
}
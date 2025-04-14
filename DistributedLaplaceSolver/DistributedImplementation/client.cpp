#include <iostream>
#include <vector>
#include <cmath>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iomanip>
using namespace std;

const int N = 200;
const int MAX_ITER = 10000;

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

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " start_row end_row\n";
        return 1;
    }

    const int start = atoi(argv[1]), end = atoi(argv[2]);
    vector<vector<double>> grid(N, vector<double>(N, 0.0));
    
    // Initialize fixed boundaries
    if (start == 0) fill(grid[0].begin(), grid[0].end(), 5.0);
    if (end == N-1) fill(grid[N-1].begin(), grid[N-1].end(), -5.0);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serv_addr = {AF_INET, htons(8080)};
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);
    connect(sock, (sockaddr*)&serv_addr, sizeof(serv_addr));

    for (int it = 0; it < MAX_ITER; it++) {
        vector<vector<double>> newGrid = grid;

        // Compute updates with boundary checks
        for (int i = max(1, start); i <= min(N-2, end); i++) {
            for (int j = 1; j < N-1; j++) {
                newGrid[i][j] = 0.25 * (
                    grid[i-1][j] + 
                    grid[i+1][j] + 
                    grid[i][j-1] + 
                    grid[i][j+1]
                );
            }
        }

        // Boundary exchange
        double send_boundary[N], recv_boundary[N];
        int send_row = (end == 99) ? 99 : 100;
        copy(newGrid[send_row].begin(), newGrid[send_row].end(), send_boundary);
        write(sock, send_boundary, sizeof(send_boundary));
        readAll(sock, recv_boundary, sizeof(recv_boundary));

        // Update ghost cells
        int recv_row = (end == 99) ? 100 : 99;
        copy(recv_boundary, recv_boundary + N, newGrid[recv_row].begin());

        // Maintain fixed boundaries
        if (start == 0) fill(newGrid[0].begin(), newGrid[0].end(), 5.0);
        if (end == N-1) fill(newGrid[N-1].begin(), newGrid[N-1].end(), -5.0);

        grid.swap(newGrid);
    }

    // Send final data
    for (int i = start; i <= end; i++) 
        write(sock, &grid[i][0], N*sizeof(double));
    
    close(sock);
    cout << "[Client " << start << "-" << end << "] Completed successfully\n";
    return 0;
}
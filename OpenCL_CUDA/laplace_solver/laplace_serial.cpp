// filepath: OpenCL_CUDA/laplace_solver/laplace_serial.cpp
#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <chrono>

void laplace_serial(std::vector<float>& grid, int N, int max_iter) {
    // Set boundary conditions: top = 5V, bottom = 5V, left = right = 0V
    for (int j = 0; j < N; ++j) {
        grid[j] = 5.0f; // top
        grid[(N-1)*N + j] = 5.0f; // bottom
    }
    for (int i = 0; i < N; ++i) {
        grid[i*N] = 0.0f; // left
        grid[i*N + N-1] = 0.0f; // right
    }

    std::vector<float> new_grid = grid;
    for (int it = 0; it < max_iter; ++it) {
        for (int i = 1; i < N-1; ++i) {
            for (int j = 1; j < N-1; ++j) {
                new_grid[i*N + j] = 0.25f * (grid[(i-1)*N + j] + grid[(i+1)*N + j] +
                                            grid[i*N + (j-1)] + grid[i*N + (j+1)]);
            }
        }
        std::swap(grid, new_grid);
    }
}

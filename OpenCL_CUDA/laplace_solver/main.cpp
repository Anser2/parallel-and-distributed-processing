// filepath: OpenCL_CUDA/laplace_solver/main.cpp
#include <iostream>
#include <vector>
#include <chrono>

void laplace_serial(std::vector<float>& grid, int N, int max_iter);
void laplace_opencl(std::vector<float>& grid, int N, int max_iter);
void laplace_cuda(std::vector<float>& grid, int N, int max_iter);

void set_boundaries(std::vector<float>& grid, int N) {
    for (int j = 0; j < N; ++j) {
        grid[j] = 5.0f;
        grid[(N-1)*N + j] = 5.0f;
    }
    for (int i = 0; i < N; ++i) {
        grid[i*N] = 0.0f;
        grid[i*N + N-1] = 0.0f;
    }
}

bool compare(const std::vector<float>& a, const std::vector<float>& b, float tol=1e-3f) {
    for (size_t i = 0; i < a.size(); ++i) {
        if (std::abs(a[i] - b[i]) > tol) return false;
    }
    return true;
}

int main() {
    const int N = 200;
    const int max_iter = 1000;
    std::vector<float> grid_serial(N*N, 0.0f);
    std::vector<float> grid_opencl(N*N, 0.0f);
    std::vector<float> grid_cuda(N*N, 0.0f);

    set_boundaries(grid_serial, N);
    set_boundaries(grid_opencl, N);
    set_boundaries(grid_cuda, N);

    auto t1 = std::chrono::high_resolution_clock::now();
    laplace_serial(grid_serial, N, max_iter);
    auto t2 = std::chrono::high_resolution_clock::now();
    std::cout << "Serial time: " << std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count() << " ms\n";

    t1 = std::chrono::high_resolution_clock::now();
    laplace_opencl(grid_opencl, N, max_iter);
    t2 = std::chrono::high_resolution_clock::now();
    std::cout << "OpenCL time: " << std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count() << " ms\n";

    t1 = std::chrono::high_resolution_clock::now();
    laplace_cuda(grid_cuda, N, max_iter);
    t2 = std::chrono::high_resolution_clock::now();
    std::cout << "CUDA time: " << std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count() << " ms\n";

    std::cout << "OpenCL correct: " << (compare(grid_serial, grid_opencl) ? "yes" : "no") << std::endl;
    std::cout << "CUDA correct: " << (compare(grid_serial, grid_cuda) ? "yes" : "no") << std::endl;

    return 0;
}

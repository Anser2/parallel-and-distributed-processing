#include <iostream>
#include <vector>
#include <chrono>
#include "matmul.hpp"
#include "utils.hpp"

int main() {
    const int N = 1024; // Size of the matrices
    std::vector<float> A(N * N), B(N * N), C(N * N);
    
    // Initialize matrices
    initialize_matrix(A.data(), N, N);
    initialize_matrix(B.data(), N, N);

    // Single-threaded multiplication
    auto start = std::chrono::high_resolution_clock::now();
    matmul_single(A.data(), B.data(), C.data(), N);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Single-threaded time: " 
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() 
              << " ms" << std::endl;

    // Multi-threaded multiplication using OpenMP
    start = std::chrono::high_resolution_clock::now();
    matmul_openmp(A.data(), B.data(), C.data(), N);
    end = std::chrono::high_resolution_clock::now();
    std::cout << "Multi-threaded time: " 
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() 
              << " ms" << std::endl;

    // OpenCL multiplication
    start = std::chrono::high_resolution_clock::now();
    matmul_opencl(A.data(), B.data(), C.data(), N);
    end = std::chrono::high_resolution_clock::now();
    std::cout << "OpenCL time: " 
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() 
              << " ms" << std::endl;

    // CUDA multiplication
    start = std::chrono::high_resolution_clock::now();
    matmul_cuda(A.data(), B.data(), C.data(), N);
    end = std::chrono::high_resolution_clock::now();
    std::cout << "CUDA time: " 
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() 
              << " ms" << std::endl;

    // Validate results
    if (validate_result(C.data(), C.data(), N, N)) {
        std::cout << "Results are correct!" << std::endl;
    } else {
        std::cout << "Results are incorrect!" << std::endl;
    }

    return 0;
}
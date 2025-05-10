// This header file declares the functions and types used in the matrix multiplication implementations.
// It includes function prototypes for all matrix multiplication methods.

#ifndef MATMUL_HPP
#define MATMUL_HPP

#include <vector>

// Function prototypes for single-threaded matrix multiplication
void matmul_single(const float* A, const float* B, float* C, int N);

// Function prototypes for multi-threaded matrix multiplication using OpenMP
void matmul_openmp(const float* A, const float* B, float* C, int N);

// Function prototypes for OpenCL matrix multiplication
void matmul_opencl(const float* A, const float* B, float* C, int N);

// Function prototypes for CUDA matrix multiplication
void matmul_cuda(const float* A, const float* B, float* C, int N);

// Utility function to validate the results of matrix multiplication
bool validate_result(const std::vector<std::vector<float>>& C1, 
                     const std::vector<std::vector<float>>& C2, 
                     int N);

// Utility function to initialize matrices with random values
void initialize_matrix(std::vector<std::vector<float>>& matrix, int N);

#endif // MATMUL_HPP
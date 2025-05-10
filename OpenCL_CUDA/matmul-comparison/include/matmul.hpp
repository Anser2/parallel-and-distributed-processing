// This header file declares the functions and types used in the matrix multiplication implementations.
// It includes function prototypes for all matrix multiplication methods.

#ifndef MATMUL_HPP
#define MATMUL_HPP

#include <vector>

// Function prototypes for matrix multiplication implementations
void matmul_single(const std::vector<std::vector<float>>& A, 
                   const std::vector<std::vector<float>>& B, 
                   std::vector<std::vector<float>>& C, 
                   int N);

void matmul_openmp(const std::vector<std::vector<float>>& A, 
                   const std::vector<std::vector<float>>& B, 
                   std::vector<std::vector<float>>& C, 
                   int N);

void matmul_opencl(const std::vector<std::vector<float>>& A, 
                   const std::vector<std::vector<float>>& B, 
                   std::vector<std::vector<float>>& C, 
                   int N);

void matmul_cuda(const std::vector<std::vector<float>>& A, 
                 const std::vector<std::vector<float>>& B, 
                 std::vector<std::vector<float>>& C, 
                 int N);

// Utility function to initialize matrices
void initialize_matrices(std::vector<std::vector<float>>& A, 
                         std::vector<std::vector<float>>& B, 
                         int N);

// Utility function to validate the results of matrix multiplication
bool validate_result(const std::vector<std::vector<float>>& C1, 
                     const std::vector<std::vector<float>>& C2, 
                     int N);

// Utility function to measure performance
double measure_performance(void (*func)(const std::vector<std::vector<float>>&,
                                        const std::vector<std::vector<float>>&,
                                        std::vector<std::vector<float>>&,
                                        int), 
                           const std::vector<std::vector<float>>& A, 
                           const std::vector<std::vector<float>>& B, 
                           std::vector<std::vector<float>>& C, 
                           int N);

#endif // MATMUL_HPP
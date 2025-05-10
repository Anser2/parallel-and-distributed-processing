// This file implements a multi-threaded version of matrix multiplication using OpenMP.
// It includes directives to parallelize the computation and functions for initializing matrices.

#include <omp.h>
#include "matmul.hpp"
#define RAND_MAX 10
#include <cmath>

// Function to perform matrix multiplication using OpenMP
void matmul_openmp(const float* A, const float* B, float* C, int N) {
    #pragma omp parallel for
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            C[i * N + j] = 0.0f;
            for (int k = 0; k < N; ++k) {
                C[i * N + j] += A[i * N + k] * B[k * N + j];
            }
        }
    }
}

// Function to initialize matrices (optional, can be included in utils.cpp)
void initialize_matrices(float* A, float* B, int N) {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            A[i * N + j] = static_cast<float>(rand()) / RAND_MAX;
            B[i * N + j] = static_cast<float>(rand()) / RAND_MAX;
        }
    }
}
// This file contains the implementation of a naive single-threaded matrix multiplication function.
// It includes functions for initializing matrices and performing the multiplication.

#include "matmul.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>

void matmul_single(const float* A, const float* B, float* C, int N) {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            C[i * N + j] = 0.0f;
            for (int k = 0; k < N; ++k) {
                C[i * N + j] += A[i * N + k] * B[k * N + j];
            }
        }
    }
}

void validate_result(const float* C, const float* C_ref, int N) {
    for (int i = 0; i < N * N; ++i) {
        if (fabs(C[i] - C_ref[i]) > 1e-5) {
            std::cerr << "Validation failed at index " << i << ": "
                      << C[i] << " != " << C_ref[i] << std::endl;
            return;
        }
    }
    std::cout << "Validation succeeded!" << std::endl;
}
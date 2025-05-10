// This file contains utility functions for matrix initialization, validation, and performance measurement. 
// It helps in comparing the results of different implementations.

#include "matmul.hpp"
#include <iostream>
#include <chrono>
#include <cstdlib>
#include <cmath>
#include <functional>

void initialize_matrix(float* matrix, int rows, int cols) {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            matrix[i * cols + j] = static_cast<float>(rand()) / RAND_MAX;
        }
    }
}

bool validate_result(float* result, float* expected, int rows, int cols) {
    for (int i = 0; i < rows; ++i) {
        for (int j = i; j < cols; ++j) {
            if (fabs(result[i * cols + j] - expected[i * cols + j]) > 1e-5) {
                return false;
            }
        }
    }
    return true;
}

double measure_time(std::function<void()> func) {
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    return duration.count();
}
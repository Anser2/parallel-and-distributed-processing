#include <cmath>
#include <functional>
#include "utils.h"
#include <iostream>
#include <vector>
#include <random>
#include <chrono>

void initialize_matrix(std::vector<std::vector<float>>& matrix, int rows, int cols) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0, 1);

    matrix.resize(rows, std::vector<float>(cols));
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            matrix[i][j] = dis(gen);
        }
    }
}

void print_matrix(const std::vector<std::vector<float>>& matrix) {
    for (const auto& row : matrix) {
        for (const auto& elem : row) {
            std::cout << elem << " ";
        }
        std::cout << std::endl;
    }
}

bool validate_result(const std::vector<std::vector<float>>& A, const std::vector<std::vector<float>>& B, const std::vector<std::vector<float>>& C) {
    int rows = A.size();
    int cols = B[0].size();
    int common_dim = B.size();

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            float sum = 0;
            for (int k = 0; k < common_dim; ++k) {
                sum += A[i][k] * B[k][j];
            }
            if (std::fabs(sum - C[i][j]) > 1e-5) {
                return false;
            }
        }
    }
    return true;
}
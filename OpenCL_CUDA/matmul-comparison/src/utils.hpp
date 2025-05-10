#pragma once

#include <functional>

void initialize_matrix(float* matrix, int rows, int cols);
bool validate_result(float* result, float* expected, int rows, int cols);
double measure_time(std::function<void()> func);

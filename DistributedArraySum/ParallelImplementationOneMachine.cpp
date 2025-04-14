//// filepath: /workspaces/parallel-and-distributed-processing/sum_openmp.cpp
#include <iostream>
#include <vector>
#include <omp.h>

int main() {
    const size_t SIZE = 1000000;
    std::vector<int> data(SIZE, 1);
    long long sum = 0;

    #pragma omp parallel for reduction(+:sum)
    for (size_t i = 0; i < SIZE; i++) {
        sum += data[i];
    }

    std::cout << "OpenMP sum = " << sum << std::endl;
    return 0;
}
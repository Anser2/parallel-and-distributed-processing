//// filepath: /workspaces/parallel-and-distributed-processing/sum_serial.cpp
#include <iostream>
#include <vector>

int main() {
    const size_t SIZE = 1000000;
    std::vector<int> data(SIZE, 1);

    long long sum = 0;
    for (size_t i = 0; i < SIZE; ++i) {
        sum += data[i];
    }

    std::cout << "Serial sum = " << sum << std::endl;
    return 0;
}
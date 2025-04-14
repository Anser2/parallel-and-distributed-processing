#include <iostream>
#include <vector>
#include <omp.h>
using namespace std;

int main() {
    const int N = 500;
    vector<vector<double>> A(N, vector<double>(N, 1.0));
    vector<vector<double>> B(N, vector<double>(N, 2.0));
    vector<vector<double>> C(N, vector<double>(N, 0.0));

    // Parallel matrix multiplication
    #pragma omp parallel for
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            double sum = 0.0;
            for(int k = 0; k < N; k++){
                sum += A[i][k] * B[k][j];
            }
            C[i][j] = sum;
        }
    }

    // Sum all elements of C in parallel
    double totalSum = 0.0;
    #pragma omp parallel for reduction(+:totalSum)
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            totalSum += C[i][j];
        }
    }

    cout << "Matrix multiplication completed (OpenMP)." << endl;
    cout << "Sum of all elements in result matrix: " << totalSum << endl;

    return 0;
}

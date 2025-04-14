#include <iostream>
#include <vector>
using namespace std;

int main() {
    const int N = 500;  // size of the square matrix
    vector<vector<double>> A(N, vector<double>(N, 1.0));
    vector<vector<double>> B(N, vector<double>(N, 2.0));
    vector<vector<double>> C(N, vector<double>(N, 0.0));

    // Serial multiplication
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            double sum = 0.0;
            for(int k = 0; k < N; k++){
                sum += A[i][k] * B[k][j];
            }
            C[i][j] = sum;
        }
    }

    // Sum of all elements in C
    double totalSum = 0.0;
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            totalSum += C[i][j];
        }
    }

    cout << "Matrix multiplication completed (serial)." << endl;
    cout << "Sum of all elements in result matrix: " << totalSum << endl;

    return 0;
}

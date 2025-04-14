#include <iostream>
#include <vector>
#include <omp.h>
#include <cmath>
#include <iomanip>
using namespace std;

int main() {
    const int N = 200;
    const int MAX_ITER = 100;
    vector<vector<double>> grid(N, vector<double>(N, 0.0));

    // Set boundary conditions
    for(int j = 0; j < N; j++){
        grid[0][j]     =  5.0; 
        grid[N-1][j]   = -5.0; 
    }

    cout << "[Init] Grid size: " << N << "x" << N << endl;
    cout << "[Init] Top boundary = +5.0, Bottom boundary = -5.0\n";
    cout << "[Init] Starting OpenMP Laplace iterations...\n";

    for(int it = 0; it < MAX_ITER; it++){
        double maxChange = 0.0;

        #pragma omp parallel for reduction(max:maxChange)
        for(int i = 1; i < N-1; i++){
            for(int j = 1; j < N-1; j++){
                double oldVal = grid[i][j];
                double newVal = 0.25 * (grid[i+1][j] + grid[i-1][j] +
                                        grid[i][j+1] + grid[i][j-1]);
                grid[i][j] = newVal;
                maxChange = max(maxChange, fabs(newVal - oldVal));
            }
        }

        if (it % 10 == 0 || it == MAX_ITER - 1) {
            cout << "[Iter " << setw(3) << it << "] Max change: " << maxChange << endl;
        }
    }

    // Final center region average (3x3)
    double centerSum = 0.0;
    for(int i = N/2 - 1; i <= N/2 + 1; i++){
        for(int j = N/2 - 1; j <= N/2 + 1; j++){
            centerSum += grid[i][j];
        }
    }

    cout << "[Result] Average center 3x3 value: " << centerSum / 9.0 << endl;
    cout << "[OpenMP] Laplace solution complete.\n";
    return 0;
}

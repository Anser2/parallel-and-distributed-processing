#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <omp.h>

#define IDX(i,j,cols) ((i)*(cols)+(j))

double get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec * 1e-6;
}

void initialize_grid(double* grid, int N) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (i == 0) grid[IDX(i,j,N)] = 100.0;
            else if (i == N-1) grid[IDX(i,j,N)] = 0.0;
            else if (j == 0) grid[IDX(i,j,N)] = 50.0;
            else if (j == N-1) grid[IDX(i,j,N)] = 75.0;
            else grid[IDX(i,j,N)] = 0.0;
        }
    }
}

int main(int argc, char** argv) {
    int N = 100;
    int max_epochs = 2000;
    double tol = 1e-6;
    int num_threads = 4;
    
    if (argc > 1) N = atoi(argv[1]);
    if (argc > 2) max_epochs = atoi(argv[2]);
    if (argc > 3) tol = atof(argv[3]);
    if (argc > 4) num_threads = atoi(argv[4]);
    
    omp_set_num_threads(num_threads);
    
    printf("=== OpenMP Laplace Solver ===\n");
    printf("Grid size: %d x %d\n", N, N);
    printf("Max epochs: %d\n", max_epochs);
    printf("Tolerance: %.2e\n", tol);
    printf("Number of threads: %d\n", num_threads);
    
    double* grid_old = (double*)calloc(N * N, sizeof(double));
    double* grid_new = (double*)calloc(N * N, sizeof(double));
    
    if (!grid_old || !grid_new) {
        printf("Memory allocation failed!\n");
        return 1;
    }
    
    initialize_grid(grid_old, N);
    initialize_grid(grid_new, N);
    
    printf("Starting computation...\n");
    
    double start_time = get_time();
    int epoch = 0;
    double final_max_diff = 0.0;
    
    do {
        double epoch_start = get_time();
        double max_diff = 0.0;
        
        #pragma omp parallel for
        for (int i = 1; i < N-1; i++) {
            for (int j = 1; j < N-1; j++) {
                grid_new[IDX(i,j,N)] = 0.25 * (
                    grid_old[IDX(i-1,j,N)] +
                    grid_old[IDX(i+1,j,N)] +
                    grid_old[IDX(i,j-1,N)] +
                    grid_old[IDX(i,j+1,N)]
                );
            }
        }
        
        for (int i = 1; i < N-1; i++) {
            for (int j = 1; j < N-1; j++) {
                double diff = fabs(grid_new[IDX(i,j,N)] - grid_old[IDX(i,j,N)]);
                if (diff > max_diff) max_diff = diff;
            }
        }
        
        double* tmp = grid_old;
        grid_old = grid_new;
        grid_new = tmp;
        
        final_max_diff = max_diff;
        double epoch_time = get_time() - epoch_start;
        
        if (epoch % 100 == 0 || epoch < 10) {
            printf("Epoch %d, MaxDiff = %.6e, Time = %.4f sec\n", 
                   epoch, max_diff, epoch_time);
        }
        
        epoch++;
        if (max_diff < tol) break;
        
    } while (epoch < max_epochs);
    
    double total_time = get_time() - start_time;
    
    printf("\n==== Final Results ====\n");
    printf("Epochs: %d\n", epoch);
    printf("Total Time: %.6f seconds\n", total_time);
    printf("Final MaxDiff: %.6e\n", final_max_diff);
    printf("Converged: %s\n", (epoch < max_epochs) ? "Yes" : "No");
    printf("Performance: %.2f epochs/sec\n", (double)epoch / total_time);
    
    printf("Center values: grid[%d,%d] = %.6f grid[%d,%d] = %.6f\n", 
           N/2, N/2, grid_old[IDX(N/2,N/2,N)], 
           N/2, N/2+1, grid_old[IDX(N/2,N/2+1,N)]);
    
    free(grid_old);
    free(grid_new);
    
    return 0;
}

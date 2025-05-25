#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

#define IDX(i,j,ncols) ((i)*(ncols)+(j))

double get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec * 1e-6;
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const int global_rows = 256;
    const int global_cols = 256;
    const int max_epochs = 1000;
    const double tol = 1e-3;

    int local_rows = global_rows / size;
    int alloc_rows = local_rows + 2;
    int alloc_cols = global_cols;

    double* grid_old = (double*)calloc(alloc_rows * alloc_cols, sizeof(double));
    double* grid_new = (double*)calloc(alloc_rows * alloc_cols, sizeof(double));

    for (int i = 1; i <= local_rows; i++) {
        for (int j = 0; j < alloc_cols; j++) {
            grid_old[IDX(i,j,alloc_cols)] = rank;
        }
    }

    double start_time = get_time();
    int epoch = 0;
    double max_diff;

    do {
        MPI_Request reqs[4];
        MPI_Status stats[4];

        if (rank > 0) {
            MPI_Isend(&grid_old[IDX(1,0,alloc_cols)], alloc_cols, MPI_DOUBLE, rank-1, 0, MPI_COMM_WORLD, &reqs[0]);
            MPI_Irecv(&grid_old[IDX(0,0,alloc_cols)], alloc_cols, MPI_DOUBLE, rank-1, 1, MPI_COMM_WORLD, &reqs[1]);
        }
        if (rank < size - 1) {
            MPI_Isend(&grid_old[IDX(local_rows,0,alloc_cols)], alloc_cols, MPI_DOUBLE, rank+1, 1, MPI_COMM_WORLD, &reqs[2]);
            MPI_Irecv(&grid_old[IDX(local_rows+1,0,alloc_cols)], alloc_cols, MPI_DOUBLE, rank+1, 0, MPI_COMM_WORLD, &reqs[3]);
        }

        MPI_Waitall(4, reqs, stats);

        max_diff = 0.0;
        double epoch_start = get_time();

        for (int i = 1; i <= local_rows; i++) {
            for (int j = 1; j < alloc_cols - 1; j++) {
                grid_new[IDX(i,j,alloc_cols)] = 0.25 * (grid_old[IDX(i-1,j,alloc_cols)] +
                                                        grid_old[IDX(i+1,j,alloc_cols)] +
                                                        grid_old[IDX(i,j-1,alloc_cols)] +
                                                        grid_old[IDX(i,j+1,alloc_cols)]);
                double diff = fabs(grid_new[IDX(i,j,alloc_cols)] - grid_old[IDX(i,j,alloc_cols)]);
                if (diff > max_diff) max_diff = diff;
            }
        }

        double* tmp = grid_old;
        grid_old = grid_new;
        grid_new = tmp;

        double global_max_diff;
        MPI_Allreduce(&max_diff, &global_max_diff, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);

        double epoch_time = get_time() - epoch_start;

        if (rank == 0) {
            printf("Epoch %d: max_diff = %e, time = %.4f sec\n", epoch, global_max_diff, epoch_time);
        }

        epoch++;
        if (global_max_diff < tol) break;

    } while (epoch < max_epochs);

    double total_time = get_time() - start_time;

    if (rank == 0) {
        printf("=== Summary ===\n");
        printf("Epochs: %d\n", epoch);
        printf("Total time: %.4f sec\n", total_time);
    }

    free(grid_old);
    free(grid_new);
    MPI_Finalize();
    return 0;
}


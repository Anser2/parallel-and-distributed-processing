### RECOMENDED ###

### Compile laplace_omp.cpp with
g++ -fopenmp -o laplace_omp laplace_omp.cpp -lm

### Run with different parameters
./laplace_omp 200 1000 1e-5 4


### Compile laplace_hpc.cpp with
mpic++ -o laplace laplace_hpc.cpp -lm

### Run with different parameters
mpirun -n 4 ./laplace 50 500 1e-6




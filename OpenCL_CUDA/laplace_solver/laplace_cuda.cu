// filepath: OpenCL_CUDA/laplace_solver/laplace_cuda.cu
#include <cuda_runtime.h>
#include <vector>
#include <iostream>

__global__ void laplace_step(float* grid, float* new_grid, int N) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    int j = blockIdx.y * blockDim.y + threadIdx.y;
    if (i > 0 && i < N-1 && j > 0 && j < N-1) {
        new_grid[i*N + j] = 0.25f * (grid[(i-1)*N + j] + grid[(i+1)*N + j] +
                                    grid[i*N + (j-1)] + grid[i*N + (j+1)]);
    }
}

void laplace_cuda(std::vector<float>& grid, int N, int max_iter) {
    float *d_grid, *d_new_grid;
    cudaError_t err;
    err = cudaMalloc(&d_grid, N*N*sizeof(float));
    if (err != cudaSuccess) { std::cerr << "CUDA: Failed to allocate d_grid\n"; return; }
    err = cudaMalloc(&d_new_grid, N*N*sizeof(float));
    if (err != cudaSuccess) { std::cerr << "CUDA: Failed to allocate d_new_grid\n"; cudaFree(d_grid); return; }
    err = cudaMemcpy(d_grid, grid.data(), N*N*sizeof(float), cudaMemcpyHostToDevice);
    if (err != cudaSuccess) { std::cerr << "CUDA: Failed to copy to d_grid\n"; cudaFree(d_grid); cudaFree(d_new_grid); return; }

    dim3 block(16, 16);
    dim3 gridDim((N+block.x-1)/block.x, (N+block.y-1)/block.y);

    for (int it = 0; it < max_iter; ++it) {
        laplace_step<<<gridDim, block>>>(d_grid, d_new_grid, N);
        err = cudaGetLastError();
        if (err != cudaSuccess) { std::cerr << "CUDA: Kernel launch failed: " << cudaGetErrorString(err) << "\n"; break; }
        std::swap(d_grid, d_new_grid);
    }
    err = cudaMemcpy(grid.data(), d_grid, N*N*sizeof(float), cudaMemcpyDeviceToHost);
    if (err != cudaSuccess) { std::cerr << "CUDA: Failed to copy to host\n"; }
    cudaFree(d_grid);
    cudaFree(d_new_grid);
}

// This file contains the CUDA version of matrix multiplication.
// It includes functions for setting up CUDA, launching kernels, and performing the multiplication on the GPU.

#include <iostream>
#include <cuda_runtime.h>
#include "matmul.hpp"

// CUDA kernel for matrix multiplication
__global__ void matmul_kernel(float *A, float *B, float *C, int N) {
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;

    if (row < N && col < N) {
        float value = 0;
        for (int k = 0; k < N; ++k) {
            value += A[row * N + k] * B[k * N + col];
        }
        C[row * N + col] = value;
    }
}

// Function to perform matrix multiplication using CUDA
void matmul_cuda(const float* h_A, const float* h_B, float* h_C, int N) {
    float *d_A, *d_B, *d_C;
    size_t size = N * N * sizeof(float);

    // Allocate device memory
    cudaMalloc(&d_A, size);
    cudaMalloc(&d_B, size);
    cudaMalloc(&d_C, size);

    // Copy matrices from host to device
    cudaMemcpy(d_A, h_A, size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_B, h_B, size, cudaMemcpyHostToDevice);

    // Define block and grid sizes
    dim3 blockSize(16, 16); // 16x16 threads per block
    dim3 gridSize((N + blockSize.x - 1) / blockSize.x, (N + blockSize.y - 1) / blockSize.y);

    // Launch the kernel
    matmul_kernel<<<gridSize, blockSize>>>(d_A, d_B, d_C, N);

    // Copy result from device to host
    cudaMemcpy(h_C, d_C, size, cudaMemcpyDeviceToHost);

    // Free device memory
    cudaFree(d_A);
    cudaFree(d_B);
    cudaFree(d_C);
}
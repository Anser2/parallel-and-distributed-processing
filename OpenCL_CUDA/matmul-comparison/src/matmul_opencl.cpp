// This file contains the OpenCL implementation of matrix multiplication.
// It includes functions for setting up the OpenCL environment, compiling the kernel,
// and executing the matrix multiplication on the GPU.

#include <CL/cl.h>
#include <iostream>
#include <vector>
#include "matmul.hpp"\

#define RAND_MAX 10

// OpenCL kernel source code
const char* kernelSource = R"(
__kernel void matmul_kernel(__global const float* A, 
                            __global const float* B, 
                            __global float* C, 
                            const int N) {
    int row = get_global_id(0);
    int col = get_global_id(1);
    float sum = 0.0f;

    for (int k = 0; k < N; k++) {
        sum += A[row * N + k] * B[k * N + col];
    }
    C[row * N + col] = sum;
}
)";

void matmul_opencl(const float* A, const float* B, float* C, int N) {
    cl_int err;

    // Get platform and device information
    cl_platform_id platform_id = nullptr;
    cl_device_id device_id = nullptr;
    cl_uint ret_num_devices;
    cl_uint ret_num_platforms;

    err = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
    err = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, &ret_num_devices);

    // Create an OpenCL context
    cl_context context = clCreateContext(nullptr, 1, &device_id, nullptr, nullptr, &err);

    // Create a command queue
    cl_command_queue queue = clCreateCommandQueue(context, device_id, 0, &err);

    // Create memory buffers on the device
    cl_mem A_mem = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float) * N * N, nullptr, &err);
    cl_mem B_mem = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float) * N * N, nullptr, &err);
    cl_mem C_mem = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * N * N, nullptr, &err);

    // Copy matrices A and B to the device
    err = clEnqueueWriteBuffer(queue, A_mem, CL_TRUE, 0, sizeof(float) * N * N, A, 0, nullptr, nullptr);
    err = clEnqueueWriteBuffer(queue, B_mem, CL_TRUE, 0, sizeof(float) * N * N, B, 0, nullptr, nullptr);

    // Create and compile the OpenCL program
    cl_program program = clCreateProgramWithSource(context, 1, &kernelSource, nullptr, &err);
    err = clBuildProgram(program, 1, &device_id, nullptr, nullptr, nullptr);

    // Create the OpenCL kernel
    cl_kernel kernel = clCreateKernel(program, "matmul_kernel", &err);

    // Set the kernel arguments
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&A_mem);
    err = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&B_mem);
    err = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*)&C_mem);
    err = clSetKernelArg(kernel, 3, sizeof(int), (void*)&N);

    // Define the global and local work sizes
    size_t globalWorkSize[2] = { N, N };
    size_t localWorkSize[2] = { 16, 16 }; // Workgroup size of 16x16

    // Execute the OpenCL kernel
    err = clEnqueueNDRangeKernel(queue, kernel, 2, nullptr, globalWorkSize, localWorkSize, 0, nullptr, nullptr);

    // Read the result from the device
    err = clEnqueueReadBuffer(queue, C_mem, CL_TRUE, 0, sizeof(float) * N * N, C, 0, nullptr, nullptr);

    // Clean up
    clReleaseMemObject(A_mem);
    clReleaseMemObject(B_mem);
    clReleaseMemObject(C_mem);
    clReleaseProgram(program);
    clReleaseKernel(kernel);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
}
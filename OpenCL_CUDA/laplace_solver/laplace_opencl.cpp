// filepath: OpenCL_CUDA/laplace_solver/laplace_opencl.cpp
#include <CL/cl.h>
#include <vector>
#include <iostream>
#include <chrono>

const char* laplace_kernel_src = R"(
__kernel void laplace_step(__global float* grid, __global float* new_grid, int N) {
    int i = get_global_id(0);
    int j = get_global_id(1);
    if (i > 0 && i < N-1 && j > 0 && j < N-1) {
        new_grid[i*N + j] = 0.25f * (grid[(i-1)*N + j] + grid[(i+1)*N + j] +
                                    grid[i*N + (j-1)] + grid[i*N + (j+1)]);
    }
}
)";

void laplace_opencl(std::vector<float>& grid, int N, int max_iter) {
    cl_int err;
    cl_platform_id platform;
    cl_device_id device;
    cl_context context;
    cl_command_queue queue;
    cl_program program;
    cl_kernel kernel;
    cl_mem grid_buf, new_grid_buf;

    err = clGetPlatformIDs(1, &platform, nullptr);
    if (err != CL_SUCCESS) { std::cerr << "OpenCL: Failed to get platform\n"; return; }
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, nullptr);
    if (err != CL_SUCCESS) { std::cerr << "OpenCL: Failed to get device\n"; return; }
    context = clCreateContext(nullptr, 1, &device, nullptr, nullptr, &err);
    if (err != CL_SUCCESS) { std::cerr << "OpenCL: Failed to create context\n"; return; }
    queue = clCreateCommandQueue(context, device, 0, &err);
    if (err != CL_SUCCESS) { std::cerr << "OpenCL: Failed to create command queue\n"; return; }

    program = clCreateProgramWithSource(context, 1, &laplace_kernel_src, nullptr, &err);
    if (err != CL_SUCCESS) { std::cerr << "OpenCL: Failed to create program\n"; return; }
    err = clBuildProgram(program, 1, &device, nullptr, nullptr, nullptr);
    if (err != CL_SUCCESS) {
        size_t log_size;
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, nullptr, &log_size);
        std::vector<char> log(log_size);
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size, log.data(), nullptr);
        std::cerr << "OpenCL Build Error:\n" << log.data() << std::endl;
        return;
    }
    kernel = clCreateKernel(program, "laplace_step", &err);
    if (err != CL_SUCCESS) { std::cerr << "OpenCL: Failed to create kernel\n"; return; }

    grid_buf = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float)*N*N, nullptr, &err);
    if (err != CL_SUCCESS) { std::cerr << "OpenCL: Failed to create grid buffer\n"; return; }
    new_grid_buf = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float)*N*N, nullptr, &err);
    if (err != CL_SUCCESS) { std::cerr << "OpenCL: Failed to create new_grid buffer\n"; return; }
    err = clEnqueueWriteBuffer(queue, grid_buf, CL_TRUE, 0, sizeof(float)*N*N, grid.data(), 0, nullptr, nullptr);
    if (err != CL_SUCCESS) { std::cerr << "OpenCL: Failed to write to grid buffer\n"; return; }

    std::vector<float> new_grid(N*N);
    size_t global[2] = { (size_t)N, (size_t)N };
    size_t local[2] = { 16, 16 }; // Reasonable workgroup size for 2D problems

    for (int it = 0; it < max_iter; ++it) {
        err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &grid_buf);
        err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &new_grid_buf);
        err |= clSetKernelArg(kernel, 2, sizeof(int), &N);
        if (err != CL_SUCCESS) { std::cerr << "OpenCL: Failed to set kernel args\n"; return; }
        err = clEnqueueNDRangeKernel(queue, kernel, 2, nullptr, global, local, 0, nullptr, nullptr);
        if (err != CL_SUCCESS) { std::cerr << "OpenCL: Failed to enqueue kernel\n"; return; }
        std::swap(grid_buf, new_grid_buf);
    }
    err = clEnqueueReadBuffer(queue, grid_buf, CL_TRUE, 0, sizeof(float)*N*N, grid.data(), 0, nullptr, nullptr);
    if (err != CL_SUCCESS) { std::cerr << "OpenCL: Failed to read buffer\n"; return; }

    clReleaseMemObject(grid_buf);
    clReleaseMemObject(new_grid_buf);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
}

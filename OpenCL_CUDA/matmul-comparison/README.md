# Matrix Multiplication Comparison Project

This project implements and compares different matrix multiplication techniques, including single-threaded, multi-threaded (using OpenMP), OpenCL, and CUDA. The goal is to evaluate the performance and correctness of each implementation.

## Project Structure

- `src/`: Contains the source code for the different matrix multiplication implementations.
  - `main.cpp`: Entry point of the application. Initializes matrices and calls the multiplication functions.
  - `matmul_single.cpp`: Naive single-threaded matrix multiplication implementation.
  - `matmul_openmp.cpp`: Multi-threaded matrix multiplication using OpenMP.
  - `matmul_opencl.cpp`: OpenCL implementation of matrix multiplication.
  - `matmul_cuda.cu`: CUDA implementation of matrix multiplication.
  - `matmul.hpp`: Header file declaring functions and types for matrix multiplication.
  - `utils.cpp`: Utility functions for matrix initialization, validation, and performance measurement.

- `include/`: Contains header files for external inclusion.
  - `matmul.hpp`: Duplicate of `src/matmul.hpp`.

- `opencl_kernels/`: Contains OpenCL kernel code.
  - `matmul_kernel.cl`: OpenCL kernel for matrix multiplication.

- `cuda_kernels/`: Contains CUDA kernel code.
  - `matmul_kernel.cu`: CUDA kernel for matrix multiplication.

- `CMakeLists.txt`: Configuration file for CMake to build the project.

## Building the Project

To build the project, ensure you have CMake installed and follow these steps:

1. Navigate to the project directory:
   ```
   cd matmul-comparison
   ```

2. Create a build directory:
   ```
   mkdir build && cd build
   ```

3. Run CMake to configure the project:
   ```
   cmake ..
   ```

4. Build the project:
   ```
   make
   ```

## Running the Implementations

After building the project, you can run the different implementations. The main executable will automatically run all implementations and compare their performance and correctness.

```
./matmul_comparison
```

## Performance Comparison

The project measures the execution time of each implementation and compares the results. The performance metrics include:

- Execution time for matrix multiplication.
- Correctness of the results by validating against a reference implementation.

## Justification for Workgroup Sizes

In the OpenCL and CUDA implementations, the choice of workgroup sizes is crucial for optimizing performance. The following factors are considered:

- **Occupancy**: The number of active warps or work-items should be maximized to utilize the GPU resources effectively.
- **Memory Access Patterns**: Workgroup sizes should be chosen to ensure coalesced memory access, reducing memory latency.
- **Hardware Limitations**: The maximum workgroup size is limited by the GPU architecture, and it is essential to choose sizes that fit within these constraints.

By experimenting with different workgroup sizes, the project aims to find the optimal configuration for each GPU architecture used.

## Conclusion

This project provides a comprehensive comparison of various matrix multiplication techniques, highlighting the strengths and weaknesses of each approach. The results will help in understanding the performance characteristics of CPU vs. GPU computations and guide future implementations in parallel computing.
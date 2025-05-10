cmake_minimum_required(VERSION 3.10)
project(MatMulComparison)

set(CMAKE_CXX_STANDARD 11)

# Enable CUDA language
enable_language(CUDA)

# Include directories
include_directories(include)

# Source files
set(SOURCES
    src/main.cpp
    src/matmul_single.cpp
    src/matmul_openmp.cpp
    src/matmul_opencl.cpp
    src/matmul_cuda.cu
    src/utils.cpp
)

# OpenCL
find_package(OpenCL REQUIRED)
include_directories(${OpenCL_INCLUDE_DIRS})

# Create the executable (CUDA is now enabled)
add_executable(matmul_comparison ${SOURCES})

# Link OpenCL libraries only (CUDA is handled automatically)
target_link_libraries(matmul_comparison ${OpenCL_LIBRARIES})
cmake_minimum_required(VERSION 3.10)
project(LaplaceSolverComparison)

set(CMAKE_CXX_STANDARD 11)

# Enable CUDA language for CUDA source files
enable_language(CUDA)

# Find OpenCL
find_package(OpenCL REQUIRED)

# Source files
set(SOURCES
    main.cpp
    laplace_serial.cpp
    laplace_opencl.cpp
    laplace_cuda.cu
)

# Include current directory
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Create executable
add_executable(laplace_solver ${SOURCES})

# Link OpenCL (CUDA is handled automatically)
target_link_libraries(laplace_solver ${OpenCL_LIBRARIES})

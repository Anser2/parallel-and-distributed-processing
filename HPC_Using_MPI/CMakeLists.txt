cmake_minimum_required(VERSION 3.10)
project(laplace_mpi)

find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})

add_executable(laplace laplace_hpc_2.cpp)
target_link_libraries(laplace ${MPI_LIBRARIES})
set_target_properties(laplace PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED YES
)


CXX = g++
CXXFLAGS = -w -O2 -std=c++17 -lstdc++fs  # Force build, silence all warnings

# ===== Laplace Solver =====
laplace_server: DistributedLaplaceSolver/DistributedImplementation/server.cpp
	$(CXX) $(CXXFLAGS) $< -o laplace_server

laplace_client: DistributedLaplaceSolver/DistributedImplementation/client.cpp
	$(CXX) $(CXXFLAGS) $< -o laplace_client

laplace_serial: DistributedLaplaceSolver/SerialImplementation.cpp
	$(CXX) $(CXXFLAGS) $< -o laplace_serial

laplace_parallel: DistributedLaplaceSolver/ParallelImplementationOneMachine.cpp
	$(CXX) $(CXXFLAGS) $< -o laplace_parallel


# ===== Matrix Multiplication =====
matmul_server: DistributedMatMul/DistributedImplementation/server.cpp
	$(CXX) $(CXXFLAGS) $< -o matmul_server

matmul_client: DistributedMatMul/DistributedImplementation/client.cpp
	$(CXX) $(CXXFLAGS) $< -o matmul_client

matmul_serial: DistributedMatMul/SerialImplementation.cpp
	$(CXX) $(CXXFLAGS) $< -o matmul_serial

matmul_parallel: DistributedMatMul/ParallelImplementationOneMachine.cpp
	$(CXX) $(CXXFLAGS) $< -o matmul_parallel


# ===== Array Sum =====
sum_server: DistributedArraySum/DistributedImplementation/server.cpp
	$(CXX) $(CXXFLAGS) $< -o sum_server

sum_client: DistributedArraySum/DistributedImplementation/client.cpp
	$(CXX) $(CXXFLAGS) $< -o sum_client

sum_serial: DistributedArraySum/SerialImplementation.cpp
	$(CXX) $(CXXFLAGS) $< -o sum_serial

sum_parallel: DistributedArraySum/ParallelImplementationOneMachine.cpp
	$(CXX) $(CXXFLAGS) $< -o sum_parallel


# ===== Batch Targets =====
all: laplace_server laplace_client laplace_serial laplace_parallel \
     matmul_server matmul_client matmul_serial matmul_parallel \
     sum_server sum_client sum_serial sum_parallel

.PHONY: clean force all

force:
	$(MAKE) -B all

clean:
	rm -f laplace_* matmul_* sum_*

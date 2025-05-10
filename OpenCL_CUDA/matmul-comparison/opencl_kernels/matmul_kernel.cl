// OpenCL kernel for naive matrix multiplication
__kernel void matmul_kernel(__global const float* A, 
                             __global const float* B, 
                             __global float* C, 
                             const int N) {
    int row = get_global_id(0);
    int col = get_global_id(1);
    
    float value = 0.0f;
    for (int k = 0; k < N; k++) {
        value += A[row * N + k] * B[k * N + col];
    }
    C[row * N + col] = value;
}
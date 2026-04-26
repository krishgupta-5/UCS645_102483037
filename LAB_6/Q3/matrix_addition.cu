%%writefile matrix_add.cu
#include <stdio.h>
#include <chrono>

__global__ void matrixAdd(int *A, int *B, int *C, int width) {
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;

    if (row < width && col < width) {
        int index = row * width + col;
        C[index] = A[index] + B[index];
    }
}

int main() {
    int width = 1024;
    int size = width * width;
    int bytes = size * sizeof(int);

    int *A = (int*)malloc(bytes);
    int *B = (int*)malloc(bytes);
    int *C = (int*)malloc(bytes);

    for (int i = 0; i < size; i++) {
        A[i] = 1;
        B[i] = 2;
    }

    int *d_A, *d_B, *d_C;
    cudaMalloc(&d_A, bytes);
    cudaMalloc(&d_B, bytes);
    cudaMalloc(&d_C, bytes);

    cudaMemcpy(d_A, A, bytes, cudaMemcpyHostToDevice);
    cudaMemcpy(d_B, B, bytes, cudaMemcpyHostToDevice);

    dim3 block(16, 16);
    dim3 grid((width + 15) / 16, (width + 15) / 16);

    auto start = std::chrono::high_resolution_clock::now();

    matrixAdd<<<grid, block>>>(d_A, d_B, d_C, width);
    cudaDeviceSynchronize();

    auto end = std::chrono::high_resolution_clock::now();

    cudaMemcpy(C, d_C, bytes, cudaMemcpyDeviceToHost);

    std::chrono::duration<double> diff = end - start;

    printf("Matrix addition done\n");
    printf("Time (GPU) = %f seconds\n", diff.count());

    cudaFree(d_A);
    cudaFree(d_B);
    cudaFree(d_C);

    free(A); free(B); free(C);

    return 0;
}
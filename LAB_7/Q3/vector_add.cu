%%writefile vector_add.cu
#include <stdio.h>
#include <cuda.h>

#define SIZE (1<<20)  

__global__ void vectorAdd(float *A, float *B, float *C) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i < SIZE)
        C[i] = A[i] + B[i];
}

int main() {

    float *h_A, *h_B, *h_C;
    h_A = (float*)malloc(SIZE * sizeof(float));
    h_B = (float*)malloc(SIZE * sizeof(float));
    h_C = (float*)malloc(SIZE * sizeof(float));

    // Initialize data
    for (int i = 0; i < SIZE; i++) {
        h_A[i] = 1.0f;
        h_B[i] = 2.0f;
    }

    float *d_A, *d_B, *d_C;

    cudaMalloc(&d_A, SIZE * sizeof(float));
    cudaMalloc(&d_B, SIZE * sizeof(float));
    cudaMalloc(&d_C, SIZE * sizeof(float));

    cudaMemcpy(d_A, h_A, SIZE * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(d_B, h_B, SIZE * sizeof(float), cudaMemcpyHostToDevice);

    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);

    cudaEventRecord(start);

    int blockSize = 256;
    int gridSize = (SIZE + blockSize - 1) / blockSize;

    vectorAdd<<<gridSize, blockSize>>>(d_A, d_B, d_C);

    cudaEventRecord(stop);
    cudaEventSynchronize(stop);

    float ms;
    cudaEventElapsedTime(&ms, start, stop);

    cudaMemcpy(h_C, d_C, SIZE * sizeof(float), cudaMemcpyDeviceToHost);

    printf("C[0] = %f\n", h_C[0]);
    printf("Kernel Time = %f ms\n", ms);

    cudaDeviceProp prop;
    cudaGetDeviceProperties(&prop, 0);

    float memoryClock = prop.memoryClockRate * 1000.0; // Hz
    float busWidth = prop.memoryBusWidth;

    float theoreticalBW = 2.0 * memoryClock * (busWidth / 8.0) / 1e9;

    printf("Theoretical Bandwidth = %f GB/s\n", theoreticalBW);

    float RBytes = SIZE * sizeof(float) * 2;
    float WBytes = SIZE * sizeof(float);

    float time_sec = ms / 1000.0;

    float measuredBW = (RBytes + WBytes) / (time_sec * 1e9);

    printf("Measured Bandwidth = %f GB/s\n", measuredBW);

    cudaFree(d_A);
    cudaFree(d_B);
    cudaFree(d_C);

    free(h_A);
    free(h_B);
    free(h_C);

    return 0;
}
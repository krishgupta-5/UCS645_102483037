%%writefile array_sum.cu
#include <stdio.h>
#include <chrono>

__global__ void sumKernel(float *input, float *result, int n) {
    __shared__ float partial[256];

    int tid = threadIdx.x;
    int i = blockIdx.x * blockDim.x + threadIdx.x;

    partial[tid] = (i < n) ? input[i] : 0;
    __syncthreads();

    for (int stride = blockDim.x / 2; stride > 0; stride /= 2) {
        if (tid < stride)
            partial[tid] += partial[tid + stride];
        __syncthreads();
    }

    if (tid == 0)
        atomicAdd(result, partial[0]);
}

int main() {
    int n = 1 << 20; // ~1M elements
    float *h_input = (float*)malloc(n * sizeof(float));
    float h_result = 0;

    for (int i = 0; i < n; i++)
        h_input[i] = 1.0f;

    float *d_input, *d_result;
    cudaMalloc((void**)&d_input, n * sizeof(float));
    cudaMalloc((void**)&d_result, sizeof(float));

    cudaMemcpy(d_input, h_input, n * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(d_result, &h_result, sizeof(float), cudaMemcpyHostToDevice);

    int blockSize = 256;
    int gridSize = (n + blockSize - 1) / blockSize;

    auto start = std::chrono::high_resolution_clock::now();

    sumKernel<<<gridSize, blockSize>>>(d_input, d_result, n);
    cudaDeviceSynchronize();

    auto end = std::chrono::high_resolution_clock::now();

    cudaMemcpy(&h_result, d_result, sizeof(float), cudaMemcpyDeviceToHost);

    std::chrono::duration<double> diff = end - start;

    printf("Sum = %f\n", h_result);
    printf("Time (GPU) = %f seconds\n", diff.count());

    cudaFree(d_input);
    cudaFree(d_result);
    free(h_input);

    return 0;
}
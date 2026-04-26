%%writefile sum.cu
#include <stdio.h>
#include <cuda.h>

#define N 1024
  
__global__ void sumKernel(int *arr, int *result) {

    int tid = threadIdx.x;

    // Thread 0 → Iterative Sum
    if (tid == 0) {
        int sum = 0;
        for (int i = 0; i < N; i++) {
            sum += arr[i];
        }
        result[0] = sum;
    }

    // Thread 1 → Formula Sum
    if (tid == 1) {
        int sum = (N * (N + 1)) / 2;
        result[1] = sum;
    }
}

int main() {

    int h_arr[N];
    int h_result[2];

    int *d_arr, *d_result;

    // Step 1: Fill array with 1 to N
    for (int i = 0; i < N; i++) {
        h_arr[i] = i + 1;
    }

    // Step 2: Allocate memory on device
    cudaMalloc(&d_arr, N * sizeof(int));
    cudaMalloc(&d_result, 2 * sizeof(int));

    // Step 3: Copy data to device
    cudaMemcpy(d_arr, h_arr, N * sizeof(int), cudaMemcpyHostToDevice);

    // Step 4: Define block and grid
    dim3 block(2);
    dim3 grid(1);

    // Step 5: Launch kernel
    sumKernel<<<grid, block>>>(d_arr, d_result);

    // Step 6: Copy results back
    cudaMemcpy(h_result, d_result, 2 * sizeof(int), cudaMemcpyDeviceToHost);

    // Output
    printf("Iterative Sum = %d\n", h_result[0]);
    printf("Formula Sum = %d\n", h_result[1]);

    // Free memory
    cudaFree(d_arr);
    cudaFree(d_result);

    return 0;
}



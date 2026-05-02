
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <cuda_runtime.h>

#define CUDA_CHECK(call)                                                    \
    do {                                                                    \
        cudaError_t err = (call);                                           \
        if (err != cudaSuccess) {                                           \
            fprintf(stderr, "CUDA error at %s:%d — %s\n",                  \
                    __FILE__, __LINE__, cudaGetErrorString(err));           \
            exit(EXIT_FAILURE);                                             \
        }                                                                   \
    } while (0)

#define N_DEFAULT (1 << 20)
#define THREADS   256



__global__ void vectorAdd(const float* A, const float* B, float* C, int N)
{
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i < N)
        C[i] = A[i] + B[i];
}

void cpu_vectorAdd(const float* A, const float* B, float* C, int N)
{
    for (int i = 0; i < N; i++)
        C[i] = A[i] + B[i];
}

static double wall_ms(void)
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec * 1e3 + t.tv_nsec * 1e-6;
}

int allclose(const float* a, const float* b, int N, float atol)
{
    for (int i = 0; i < N; i++)
        if (fabsf(a[i] - b[i]) > atol) return 0;
    return 1;
}

void run_vector_add(int N)
{
    size_t bytes = N * sizeof(float);

    float *h_A = (float*)malloc(bytes);
    float *h_B = (float*)malloc(bytes);
    float *h_C = (float*)malloc(bytes);
    float *h_ref = (float*)malloc(bytes);

    for (int i = 0; i < N; i++) {
        h_A[i] = (float)rand() / RAND_MAX;
        h_B[i] = (float)rand() / RAND_MAX;
    }

    double t0 = wall_ms();
    cpu_vectorAdd(h_A, h_B, h_ref, N);
    double cpu_ms = wall_ms() - t0;

    float *d_A, *d_B, *d_C;
    CUDA_CHECK(cudaMalloc(&d_A, bytes));
    CUDA_CHECK(cudaMalloc(&d_B, bytes));
    CUDA_CHECK(cudaMalloc(&d_C, bytes));

    CUDA_CHECK(cudaMemcpy(d_A, h_A, bytes, cudaMemcpyHostToDevice));
    CUDA_CHECK(cudaMemcpy(d_B, h_B, bytes, cudaMemcpyHostToDevice));

    printf("\n[A1-VectorAdd] N=%d  CPU=%.2f ms\n", N, cpu_ms);
    printf("Threads   Blocks   GPU(ms)   Speedup\n");

    int thread_sizes[] = {64, 128, 256, 512, 1024};

    for(int t = 0; t < 5; t++) {

        int threads = thread_sizes[t];
        int blocks  = (N + threads - 1) / threads;

        cudaEvent_t start, stop;
        cudaEventCreate(&start);
        cudaEventCreate(&stop);

        cudaEventRecord(start);
        vectorAdd<<<blocks, threads>>>(d_A, d_B, d_C, N);
        cudaEventRecord(stop);
        cudaEventSynchronize(stop);

        float gpu_ms = 0.0f;
        cudaEventElapsedTime(&gpu_ms, start, stop);

        CUDA_CHECK(cudaMemcpy(h_C, d_C, bytes, cudaMemcpyDeviceToHost));

        int ok = allclose(h_C, h_ref, N, 1e-4f);

        printf("%6d   %6d   %8.3f   %8.2f   %s\n",
               threads, blocks, gpu_ms, cpu_ms/gpu_ms,
               ok ? "[PASS]" : "[FAIL]");

        cudaEventDestroy(start);
        cudaEventDestroy(stop);
    }

    cudaFree(d_A); cudaFree(d_B); cudaFree(d_C);
    free(h_A); free(h_B); free(h_C); free(h_ref);
}



__global__ void vectorScale(const float* A, float* C, float k, int N)
{
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i < N) {
        C[i] = k * A[i];
    }
}

void diy_vector_scale(int N)
{
    size_t bytes = N * sizeof(float);
    float k = 3.14f;

    float *h_A   = (float*)malloc(bytes);
    float *h_C   = (float*)malloc(bytes);
    float *h_ref = (float*)malloc(bytes);
    for (int i = 0; i < N; i++) { h_A[i] = (float)rand() / RAND_MAX; h_ref[i] = k * h_A[i]; }

    float *d_A, *d_C;
    CUDA_CHECK(cudaMalloc(&d_A, bytes));
    CUDA_CHECK(cudaMalloc(&d_C, bytes));
    CUDA_CHECK(cudaMemcpy(d_A, h_A, bytes, cudaMemcpyHostToDevice));
    CUDA_CHECK(cudaMemset(d_C, 0, bytes));

    int threads = THREADS, blocks = (N + threads - 1) / threads;
    vectorScale<<<blocks, threads>>>(d_A, d_C, k, N);
    CUDA_CHECK(cudaMemcpy(h_C, d_C, bytes, cudaMemcpyDeviceToHost));

    int ok = allclose(h_C, h_ref, N, 1e-4f);
    printf("  [B1-VectorScale] k=%.2f  %s\n", k, ok ? "[PASS]" : "[FAIL] -- check your kernel");

    cudaFree(d_A); cudaFree(d_C);
    free(h_A); free(h_C); free(h_ref);
}


__global__ void squaredDiff(const float* A, const float* B, float* C, int N)
{
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i < N) {
        float diff = A[i] - B[i];
        C[i] = diff * diff;
    }
}

void diy_squared_diff(int N)
{
    size_t bytes = N * sizeof(float);
    float *h_A   = (float*)malloc(bytes);
    float *h_B   = (float*)malloc(bytes);
    float *h_C   = (float*)malloc(bytes);
    float *h_ref = (float*)malloc(bytes);
    for (int i = 0; i < N; i++) {
        h_A[i] = (float)rand() / RAND_MAX;
        h_B[i] = (float)rand() / RAND_MAX;
        float d = h_A[i] - h_B[i];
        h_ref[i] = d * d;
    }

    float *d_A, *d_B, *d_C;
    CUDA_CHECK(cudaMalloc(&d_A, bytes));
    CUDA_CHECK(cudaMalloc(&d_B, bytes));
    CUDA_CHECK(cudaMalloc(&d_C, bytes));
    CUDA_CHECK(cudaMemcpy(d_A, h_A, bytes, cudaMemcpyHostToDevice));
    CUDA_CHECK(cudaMemcpy(d_B, h_B, bytes, cudaMemcpyHostToDevice));
    CUDA_CHECK(cudaMemset(d_C, 0, bytes));

    int threads = THREADS, blocks = (N + threads - 1) / threads;
    squaredDiff<<<blocks, threads>>>(d_A, d_B, d_C, N);
    CUDA_CHECK(cudaMemcpy(h_C, d_C, bytes, cudaMemcpyDeviceToHost));

    int ok = allclose(h_C, h_ref, N, 1e-4f);
    printf("  [B2-SquaredDiff] %s\n", ok ? "[PASS]" : "[FAIL] -- check your kernel");

    cudaFree(d_A); cudaFree(d_B); cudaFree(d_C);
    free(h_A); free(h_B); free(h_C); free(h_ref);
}


void diy_launch_config(void)
{
    int N_values[] = {1, 100, 256, 257, 1024, 10000, 1 << 20};
    int n_cases = sizeof(N_values) / sizeof(N_values[0]);

    int thread_sizes[] = {64,128,256,512,1024};

    printf("\n[B3-LaunchConfig]\n");

    for(int t=0; t<5; t++){
        int threads = thread_sizes[t];

        printf("\nThreads per block = %d\n", threads);
        printf("N        Blocks    TotalThreads    Status\n");

        for (int c = 0; c < n_cases; c++) {
            int N = N_values[c];
            int blocks = (N + threads - 1) / threads;
            int total = blocks * threads;

            printf("%8d  %8d  %12d  %s\n",
                   N, blocks, total,
                   (total >= N) ? "[OK]" : "[FAIL]");
        }
    }
}
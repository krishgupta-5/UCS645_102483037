%%writefile pipelined.cpp
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 1000

void merge(int arr[], int temp[], int left, int mid, int right) {
    int i = left, j = mid, k = left;

    while (i < mid && j < right) {
        if (arr[i] <= arr[j])
            temp[k++] = arr[i++];
        else
            temp[k++] = arr[j++];
    }

    while (i < mid) temp[k++] = arr[i++];
    while (j < right) temp[k++] = arr[j++];

    for (i = left; i < right; i++)
        arr[i] = temp[i];
}

int main() {

    int arr[N], temp[N];

    for (int i = 0; i < N; i++)
        arr[i] = rand() % 1000;

    clock_t start = clock();

    // Pipelined (bottom-up)
    for (int width = 1; width < N; width *= 2) {
        for (int i = 0; i < N; i += 2 * width) {

            int left = i;
            int mid = (i + width < N) ? i + width : N;
            int right = (i + 2 * width < N) ? i + 2 * width : N;

            merge(arr, temp, left, mid, right);
        }
    }

    clock_t end = clock();

    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC * 1000;

    printf("CPU Pipelined Time = %f ms\n", time_taken);

    return 0;
}
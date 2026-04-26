%%writefile serial_merge.cpp
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 1000

void merge(int arr[], int l, int m, int r) {
    int i = l, j = m + 1, k = 0;
    int temp[r - l + 1];

    while (i <= m && j <= r) {
        if (arr[i] <= arr[j])
            temp[k++] = arr[i++];
        else
            temp[k++] = arr[j++];
    }

    while (i <= m) temp[k++] = arr[i++];
    while (j <= r) temp[k++] = arr[j++];

    for (i = l, k = 0; i <= r; i++, k++)
        arr[i] = temp[k];
}

void mergeSort(int arr[], int l, int r) {
    if (l < r) {
        int m = (l + r) / 2;
        mergeSort(arr, l, m);
        mergeSort(arr, m + 1, r);
        merge(arr, l, m, r);
    }
}

int main() {

    int arr[N];

    for (int i = 0; i < N; i++)
        arr[i] = rand() % 1000;

    clock_t start = clock();

    mergeSort(arr, 0, N - 1);

    clock_t end = clock();

    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC * 1000;

    printf("Serial Merge Sort Time = %f ms\n", time_taken);

    return 0;
}
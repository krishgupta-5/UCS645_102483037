%%writefile matrix_addition.cpp
#include <iostream>
#include <chrono>
using namespace std;

int main() {
    int n = 1024;
    int size = n * n;

    int *A = new int[size];
    int *B = new int[size];
    int *C = new int[size];

    for (int i = 0; i < size; i++) {
        A[i] = 1;
        B[i] = 2;
    }

    auto start = chrono::high_resolution_clock::now();

    for (int i = 0; i < size; i++) {
        C[i] = A[i] + B[i];
    }

    auto end = chrono::high_resolution_clock::now();

    chrono::duration<double> diff = end - start;

    cout << "Time (CPU Serial) = " << diff.count() << " sec\n";

    delete[] A; delete[] B; delete[] C;
    return 0;
}
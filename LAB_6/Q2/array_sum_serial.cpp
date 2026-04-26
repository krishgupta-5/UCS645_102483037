%%writefile serial_sum.cpp
#include <iostream>
#include <chrono>
using namespace std;

int main() {
    int n = 1 << 20;
    float *arr = new float[n];

    for (int i = 0; i < n; i++)
        arr[i] = 1.0;

    auto start = chrono::high_resolution_clock::now();

    float sum = 0;
    for (int i = 0; i < n; i++)
        sum += arr[i];

    auto end = chrono::high_resolution_clock::now();

    chrono::duration<double> diff = end - start;

    cout << "Sum = " << sum << endl;
    cout << "Time = " << diff.count() << " sec\n";

    delete[] arr;
    return 0;
}
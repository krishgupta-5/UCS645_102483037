#include <iostream>
#include <vector>
#include <ctime>

#define N (1<<16)

int main() {
    std::vector<double> X(N);
    std::vector<double> Y(N);

    double a = 2.5;

    for (int i = 0; i < N; i++) {
        X[i] = 1.0 + (i % 100) * 0.01;
        Y[i] = 2.0 + (i % 50) * 0.02;
    }

    clock_t start = clock();

    for (int i = 0; i < N; i++) {
        X[i] = a * X[i] + Y[i];
    }

    clock_t end = clock();

    double time_taken = static_cast<double>(end - start) / CLOCKS_PER_SEC;

    std::cout << "Q1_SERIAL TIME=" << time_taken << std::endl;

    return 0;
}
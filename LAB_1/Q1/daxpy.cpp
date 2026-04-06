#include <iostream>
#include <omp.h>
#include <vector>
using namespace std;

int main() {
    const int N = 1 << 16; // 2^16
    double a = 2.5;

    vector<double> X(N, 1.0), Y(N, 2.0);

    for (int threads = 2; threads <= 12; threads++) {
        omp_set_num_threads(threads);

        double start = omp_get_wtime();

        #pragma omp parallel for
        for (int i = 0; i < N; i++) {
            X[i] = a * X[i] + Y[i];
        }

        double end = omp_get_wtime();

        cout << threads << " " << (end - start) << "\n";
    }

    return 0;
}
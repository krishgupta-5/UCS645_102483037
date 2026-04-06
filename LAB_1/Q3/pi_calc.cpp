#include <iostream>
#include <omp.h>
using namespace std;

int main() {
    long num_steps = 1000000;
    double step = 1.0 / (double) num_steps;
    double sum = 0.0;

    double start = omp_get_wtime();

    #pragma omp parallel for reduction(+:sum)
    for (long i = 0; i < num_steps; i++) {
        double x = (i + 0.5) * step;
        sum += 4.0 / (1.0 + x * x);
    }

    double pi = step * sum;

    double end = omp_get_wtime();

    cout << "Calculated Pi: " << pi << endl;
    cout << "Time: " << (end - start) << " sec\n";

    return 0;
}
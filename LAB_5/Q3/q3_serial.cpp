#include <iostream>
#include <ctime>

#define N 500000000LL

int main() {
    double multiplier = 2.0;
    volatile double sum = 0.0;  // prevent compiler optimization
    double b = 2.0 * multiplier;

    clock_t start = clock();

    for (long long i = 0; i < N; i++) {
        sum += b;
    }

    clock_t end = clock();

    double time_taken = static_cast<double>(end - start) / CLOCKS_PER_SEC;

    std::cout << "Q3_SERIAL TIME=" << time_taken
              << " DOT=" << sum << std::endl;

    return 0;
}
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>

bool is_prime(int n) {
    if (n < 2) return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;

    int lim = static_cast<int>(std::sqrt(n));
    for (int d = 3; d <= lim; d += 2) {
        if (n % d == 0) return false;
    }
    return true;
}

int main(int argc, char** argv) {
    int maxv = (argc >= 2) ? std::atoi(argv[1]) : 200000;

    clock_t start = clock();

    int count = 0;
    for (int i = 2; i <= maxv; i++) {
        if (is_prime(i)) count++;
    }

    clock_t end = clock();

    double time_taken = static_cast<double>(end - start) / CLOCKS_PER_SEC;

    std::cout << "Q4_SERIAL TIME=" << time_taken
              << " PRIMES=" << count
              << " MAX=" << maxv << std::endl;

    return 0;
}
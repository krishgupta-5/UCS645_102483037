#include <iostream>
#include <cstdlib>
#include <ctime>

bool is_perfect(int n) {
    if (n < 2) return false;

    int sum = 1;
    for (int d = 2; d * d <= n; d++) {
        if (n % d == 0) {
            sum += d;
            int other = n / d;
            if (other != d) sum += other;
        }
    }
    return sum == n;
}

int main(int argc, char** argv) {
    int maxv = (argc >= 2) ? std::atoi(argv[1]) : 100000;

    clock_t start = clock();

    int count = 0;
    for (int i = 2; i <= maxv; i++) {
        if (is_perfect(i)) count++;
    }

    clock_t end = clock();

    double time_taken = static_cast<double>(end - start) / CLOCKS_PER_SEC;

    std::cout << "Q5_SERIAL TIME=" << time_taken
              << " PERFECT=" << count
              << " MAX=" << maxv << std::endl;

    return 0;
}
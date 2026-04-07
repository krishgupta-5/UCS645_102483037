#include "correlate.h"
#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <cstring>

#ifdef _OPENMP
#include <omp.h>
#endif

static void usage(const char* prog) {
    std::cerr
        << "Usage:\n"
        << "  " << prog << " <ny> <nx> <mode> [threads]\n\n"
        << "Args:\n"
        << "  ny      number of vectors (rows)\n"
        << "  nx      numbers per vector (cols)\n"
        << "  mode    0=sequential, 1=omp, 2=optimized\n"
        << "  threads optional (only for omp/opt), default = max\n";
}

int main(int argc, char** argv) {
    if (argc < 4) {
        usage(argv[0]);
        return 1;
    }

    int ny = std::stoi(argv[1]);
    int nx = std::stoi(argv[2]);
    int mode = std::stoi(argv[3]);

    int threads = 0;
    if (argc >= 5) threads = std::stoi(argv[4]);

#ifdef _OPENMP
    if (threads > 0) omp_set_num_threads(threads);
#endif

    std::vector<float> data((size_t)ny * (size_t)nx);
    std::mt19937 rng(12345);
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    for (auto &v : data) v = dist(rng);

    std::vector<float> result((size_t)ny * (size_t)ny, 0.0f);

    auto t0 = std::chrono::high_resolution_clock::now();
    correlate(ny, nx, data.data(), result.data(), mode);
    auto t1 = std::chrono::high_resolution_clock::now();

    double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();

#ifdef _OPENMP
    int used_threads = omp_get_max_threads();
#else
    int used_threads = 1;
#endif

    std::cout << "ny=" << ny << " nx=" << nx
              << " mode=" << mode
              << " threads=" << used_threads
              << " time_ms=" << ms << "\n";

    double checksum = 0.0;
    for (int i = 0; i < ny; ++i) checksum += result[i + i * ny];
    std::cout << "checksum(diagonal_sum)=" << checksum << "\n";

    return 0;
}
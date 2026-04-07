#include <mpi.h>
#include <iostream>
#include <cstdlib>

#define TOTAL_N 500000000LL

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    double multiplier = 2.0;
    if (rank == 0 && argc >= 2) {
        multiplier = std::atof(argv[1]);
    }

    MPI_Bcast(&multiplier, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    long long base = TOTAL_N / size;
    long long rem  = TOTAL_N % size;
    long long local_n = base + (rank < rem ? 1 : 0);

    MPI_Barrier(MPI_COMM_WORLD);
    double start = MPI_Wtime();

    double local_dot = 0.0;
    double b = 2.0 * multiplier;

    for (long long i = 0; i < local_n; i++) {
        local_dot += b;
    }

    double global_dot = 0.0;
    MPI_Reduce(&local_dot, &global_dot, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    double end = MPI_Wtime();

    double local_time = end - start;
    double max_time = 0.0;

    MPI_Reduce(&local_time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        std::cout << "Q3 NP=" << size
                  << " TIME=" << max_time
                  << " DOT=" << global_dot << std::endl;
    }

    MPI_Finalize();
    return 0;
}
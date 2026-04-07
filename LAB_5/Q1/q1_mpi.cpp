#include <mpi.h>
#include <iostream>
#include <vector>

#define N (1<<16)

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    double a = 2.5;

    if (N % size != 0) {
        if (rank == 0) {
            std::cout << "Q1 ERROR: N=" << N
                      << " not divisible by NP=" << size << std::endl;
        }
        MPI_Finalize();
        return 1;
    }

    int local_n = N / size;

    std::vector<double> X, Y;
    if (rank == 0) {
        X.resize(N);
        Y.resize(N);

        for (int i = 0; i < N; i++) {
            X[i] = 1.0 + (i % 100) * 0.01;
            Y[i] = 2.0 + (i % 50) * 0.02;
        }
    }

    std::vector<double> localX(local_n);
    std::vector<double> localY(local_n);

    MPI_Scatter(X.data(), local_n, MPI_DOUBLE,
                localX.data(), local_n, MPI_DOUBLE,
                0, MPI_COMM_WORLD);

    MPI_Scatter(Y.data(), local_n, MPI_DOUBLE,
                localY.data(), local_n, MPI_DOUBLE,
                0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    double start = MPI_Wtime();

    for (int i = 0; i < local_n; i++) {
        localX[i] = a * localX[i] + localY[i];
    }

    MPI_Barrier(MPI_COMM_WORLD);
    double end = MPI_Wtime();

    double local_time = end - start;
    double max_time = 0.0;

    MPI_Reduce(&local_time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    MPI_Gather(localX.data(), local_n, MPI_DOUBLE,
               X.data(), local_n, MPI_DOUBLE,
               0, MPI_COMM_WORLD);

    if (rank == 0) {
        std::cout << "Q1 NP=" << size
                  << " TIME=" << max_time << std::endl;
    }

    MPI_Finalize();
    return 0;
}
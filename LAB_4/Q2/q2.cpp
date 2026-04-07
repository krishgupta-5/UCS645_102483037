#include <mpi.h>
#include <iostream>
#include <vector>

#define N 100

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    std::cout.setf(std::ios::unitbuf); // unbuffered output

    int local_n = N / size;  // assume size divides N

    std::vector<int> data;
    std::vector<int> local(local_n);

    if (rank == 0) {
        data.resize(N);
        for (int i = 0; i < N; i++) {
            data[i] = i + 1;
        }
    }

    MPI_Scatter(
        data.data(), local_n, MPI_INT,
        local.data(), local_n, MPI_INT,
        0, MPI_COMM_WORLD
    );

    long long local_sum = 0;
    for (int i = 0; i < local_n; i++) {
        local_sum += local[i];
    }

    long long global_sum = 0;
    MPI_Reduce(
        &local_sum, &global_sum,
        1, MPI_LONG_LONG, MPI_SUM,
        0, MPI_COMM_WORLD
    );

    if (rank == 0) {
        double avg = static_cast<double>(global_sum) / N;

        std::cout << "Global Sum = " << global_sum << " (Expected 5050)" << std::endl;
        std::cout << "Average    = " << avg << std::endl;
    }

    MPI_Finalize();
    return 0;
}
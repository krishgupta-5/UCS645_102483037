#include <mpi.h>
#include <iostream>
#include <vector>

#define N 10000000

void MyBcast(double* arr, int count, int root, MPI_Comm comm) {
    int rank, size;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);

    if (rank == root) {
        for (int i = 1; i < size; i++) {
            MPI_Send(arr, count, MPI_DOUBLE, i, 0, comm);
        }
    } else {
        MPI_Recv(arr, count, MPI_DOUBLE, root, 0, comm, MPI_STATUS_IGNORE);
    }
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    std::vector<double> arr(N);

    // Part A: manual broadcast
    if (rank == 0)
        for (int i = 0; i < N; i++) arr[i] = 1.2345;
    else
        for (int i = 0; i < N; i++) arr[i] = 0.0;

    MPI_Barrier(MPI_COMM_WORLD);
    double s1 = MPI_Wtime();

    MyBcast(arr.data(), N, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    double e1 = MPI_Wtime();
    double t_manual = e1 - s1;

    // Part B: MPI_Bcast
    if (rank == 0)
        for (int i = 0; i < N; i++) arr[i] = 9.8765;
    else
        for (int i = 0; i < N; i++) arr[i] = 0.0;

    MPI_Barrier(MPI_COMM_WORLD);
    double s2 = MPI_Wtime();

    MPI_Bcast(arr.data(), N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    double e2 = MPI_Wtime();
    double t_bcast = e2 - s2;

    double manual_max = 0.0, bcast_max = 0.0;

    MPI_Reduce(&t_manual, &manual_max, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    MPI_Reduce(&t_bcast, &bcast_max, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        std::cout << "Q2 NP=" << size
                  << " MANUAL=" << manual_max
                  << " MPI_BCAST=" << bcast_max << std::endl;
    }

    MPI_Finalize();
    return 0;
}
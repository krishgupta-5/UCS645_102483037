#include <mpi.h>
#include <iostream>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    std::cout.setf(std::ios::unitbuf); // unbuffered output

    int value;
    int next = (rank + 1) % size;
    int prev = (rank - 1 + size) % size;

    if (rank == 0) {
        value = 100;
        std::cout << "Rank 0 starts with value " << value << std::endl;

        MPI_Send(&value, 1, MPI_INT, next, 0, MPI_COMM_WORLD);
        MPI_Recv(&value, 1, MPI_INT, prev, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        std::cout << "Rank 0 received final value " << value << std::endl;
    } else {
        MPI_Recv(&value, 1, MPI_INT, prev, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        value += rank;
        std::cout << "Rank " << rank << " updated value to " << value << std::endl;

        MPI_Send(&value, 1, MPI_INT, next, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
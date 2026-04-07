#include <mpi.h>
#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>

#define K 10
#define MAXVAL 1000

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    std::cout.setf(std::ios::unitbuf); // unbuffered output

    // random seed per rank
    unsigned int seed = static_cast<unsigned int>(time(NULL)) ^ (rank * 1234567u);

    std::vector<int> nums(K);
    for (int i = 0; i < K; i++) {
        nums[i] = rand_r(&seed) % (MAXVAL + 1);
    }

    int local_max = nums[0], local_min = nums[0];
    for (int i = 1; i < K; i++) {
        if (nums[i] > local_max) local_max = nums[i];
        if (nums[i] < local_min) local_min = nums[i];
    }

    // Pair struct for MPI_MAXLOC / MPI_MINLOC
    struct {
        int value;
        int rank;
    } in_max, out_max, in_min, out_min;

    in_max.value = local_max;
    in_max.rank = rank;

    in_min.value = local_min;
    in_min.rank = rank;

    MPI_Reduce(&in_max, &out_max, 1, MPI_2INT, MPI_MAXLOC, 0, MPI_COMM_WORLD);
    MPI_Reduce(&in_min, &out_min, 1, MPI_2INT, MPI_MINLOC, 0, MPI_COMM_WORLD);

    // Print local stats
    std::cout << "Rank " << rank
              << " local_min=" << local_min
              << " local_max=" << local_max << std::endl;

    if (rank == 0) {
        std::cout << "\nGLOBAL MAX = " << out_max.value
                  << " (from Rank " << out_max.rank << ")" << std::endl;

        std::cout << "GLOBAL MIN = " << out_min.value
                  << " (from Rank " << out_min.rank << ")" << std::endl;
    }

    MPI_Finalize();
    return 0;
}
#include <mpi.h>
#include <iostream>
#include <cmath>
#include <cstdlib>

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
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2) {
        if (rank == 0)
            std::cout << "Q4 ERROR: run with NP>=2" << std::endl;
        MPI_Finalize();
        return 1;
    }

    int maxv = 200000;
    if (rank == 0 && argc >= 2)
        maxv = std::atoi(argv[1]);

    MPI_Bcast(&maxv, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        double start = MPI_Wtime();

        int next = 2;
        int active = size - 1;
        int primes = 0;

        while (active > 0) {
            int msg;
            MPI_Status st;

            MPI_Recv(&msg, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &st);
            int src = st.MPI_SOURCE;

            if (msg > 0) primes++;

            if (next <= maxv) {
                MPI_Send(&next, 1, MPI_INT, src, 0, MPI_COMM_WORLD);
                next++;
            } else {
                int stop = 0;
                MPI_Send(&stop, 1, MPI_INT, src, 0, MPI_COMM_WORLD);
                active--;
            }
        }

        double end = MPI_Wtime();

        std::cout << "Q4 NP=" << size
                  << " TIME=" << (end - start)
                  << " PRIMES=" << primes
                  << " MAX=" << maxv << std::endl;

    } else {
        int req = 0;
        MPI_Send(&req, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

        while (true) {
            int n;
            MPI_Recv(&n, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            if (n == 0) break;

            int res = is_prime(n) ? n : -n;
            MPI_Send(&res, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }
    }

    MPI_Finalize();
    return 0;
}
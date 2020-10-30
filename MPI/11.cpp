#include <iostream>
#include <mpi.h>

int main(int argc, char **argv) {
    int rank, size;
    MPI_Status status;
    int num;
    const int limit = 20;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0)
        MPI_Send(&num, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
    int i = 1;
    for (int l = 0; l < limit; l++) {
        if (rank == i) {
            int prev_i = i == 0 ? size - 1 : i - 1;
            MPI_Recv(&num, 1, MPI_INT, prev_i, prev_i, MPI_COMM_WORLD, &status);
            printf("Rank = %d, buf = %d\n", rank, num);
            num = num + 1;
            MPI_Send(&num, 1, MPI_INT, (i + 1) % size, i, MPI_COMM_WORLD);
        }
        i = (i + 1) % size;
    }

    MPI_Finalize();
    return 0;
}
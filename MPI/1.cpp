#include <iostream>
#include <mpi.h>

int main(int argc, char **argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    printf("My number is %d of %d, Hello World!\n", rank, size);
    MPI_Finalize();
    return 0;
}
#include <iostream>
#include <mpi.h>

int main(int argc, char **argv) {
    srand (time(NULL));
    int rank, size;
    int vector_size = 16;
    int vector[vector_size];
    int result;
    int localmax;
    for (int i = 0; i < 16; i++)
        vector[i] = rand() % 100;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int element_per_proc = vector_size / size;
    localmax = vector[rank * element_per_proc];
    for (int i = rank * element_per_proc + 1; i < (rank + 1) * element_per_proc; i++)
        if (vector[i] > localmax) localmax = vector[i];

    MPI_Reduce(&localmax, &result, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);

    if (rank == 0)
        printf("Max = %d\n", result);
    MPI_Finalize();
    return 0;
}
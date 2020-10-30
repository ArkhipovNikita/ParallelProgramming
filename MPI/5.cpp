#include <iostream>
#include <mpi.h>


int main(int argc, char **argv) {
    srand (time(NULL));
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // нужно ли оставлять некторые переменные по всех процессах?
    int arr_size = 10;
    int a[arr_size], b[arr_size], counts[size], displs[size];
    int *localA, *localB;
    int localProd, res;

    if (rank == 0) {
        for (int i = 0; i < arr_size; i++) {
            a[i] = rand() % 100;
            b[i] = rand() % 100;
        }
        int restElems = arr_size;
        counts[0] = arr_size / size;
        displs[0] = 0;
        for (int i = 1; i < size; i++) {
            restElems -= counts[i - 1];
            counts[i] = restElems / (size - i);
            displs[i] = displs[i - 1] + counts[i - 1];
        }
    }

    localA = new int[counts[rank]];
    localB = new int[counts[rank]];

    MPI_Bcast(counts, size, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(displs, size, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Scatterv(a, counts, displs, MPI_INT, localA, counts[rank], MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatterv(b, counts, displs, MPI_INT, localB, counts[rank], MPI_INT, 0, MPI_COMM_WORLD);

    for (int i = 0; i < counts[rank]; i++) {
        localProd += localA[i] * localB[i];
    }

    MPI_Reduce(&localProd, &res, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Prod = %d\n", res);
    }

    MPI_Finalize();
    return 0;
}
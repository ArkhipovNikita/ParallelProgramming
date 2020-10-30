#include <iostream>
#include <mpi.h>


int main(int argc, char **argv) {
    srand (time(NULL));
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // нужно ли оставлять некторые переменные по всех процессах?
    int arr_size = 100;
    int arr[arr_size], counts[size], displs[size], ns[size];
    int *localArr;
    int localSum, localN, sum;

    if (rank == 0) {
        for (int i = 0; i < arr_size; i++)
            arr[i] = rand() % 10;
        int restElems = arr_size;
        counts[0] = arr_size / size;
        displs[0] = 0;
        for (int i = 1; i < size; i++) {
            restElems -= counts[i - 1];
            counts[i] = restElems / (size - i);
            displs[i] = displs[i - 1] + counts[i - 1];
        }
    }

    MPI_Bcast(counts, size, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(displs, size, MPI_INT, 0, MPI_COMM_WORLD);

    localArr = new int[counts[rank]];

    MPI_Scatterv(arr, counts, displs, MPI_INT, localArr, counts[rank], MPI_INT, 0, MPI_COMM_WORLD);

    for (int i = 0; i < counts[rank]; i++) {
        if (localArr[i] > 0) {
            localSum += localArr[i];
            localN++;
        }
    }

    MPI_Reduce(&localSum, &sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Gather(&localN, 1, MPI_INT, ns, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        int n = 0;
        for (int i = 0; i < size; i++)
            n += ns[i];
        printf("Avg = %f\n", sum / (n * 1.0));
    }

    MPI_Finalize();
    return 0;
}
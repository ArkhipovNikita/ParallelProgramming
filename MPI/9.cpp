#include <iostream>
#include <mpi.h>

void printArray(int* arr, int size) {
    for (int i = 0; i < size; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

void RandomDataInitialization (int* array, const int length, const int rand_diapason) {
    srand (time(NULL));
    for (int i = 0; i < length; i++)
        array[i] = i; // rand() % (2 * rand_diapason) - rand_diapason
}

void swap(int *p1, int *p2) {
    int t = *p1;
    *p1 = *p2;
    *p2 = t;
}


int main(int argc, char **argv) {
    int *pArr, *pProcArr, *pResultArr;
    int rank, size;
    int length = 13;
    int root = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == root) {
        pArr = new int[length];
        pResultArr = new int[length];
        RandomDataInitialization(pArr, length, 100);
        printf("Initial array\n");
        printArray(pArr, length);
        printf("---------------\n");
    }

    int counts[size];
    int displs[size];
    counts[0] = length / size;
    displs[0] = 0;
    int restElems = length;
    for (int i = 1; i < size; i++) {
        restElems -= counts[i - 1];
        counts[i] = restElems / (size  - i);
        displs[i] = displs[i - 1] + counts[i - 1];
    }

    pProcArr = new int[counts[rank]];
    MPI_Scatterv(pArr, counts, displs, MPI_INT, pProcArr, counts[rank], MPI_INT, root, MPI_COMM_WORLD);

    printf("Subarray of process №%d\n", rank);
    printArray(pProcArr, counts[rank]);
    printf("---------------\n");

    for (int i = 0; i < counts[rank] / 2 ; i++) {
        swap(&pProcArr[i], &pProcArr[counts[rank] - 1 - i]);
    }

    printf("Inverted subarray of process №%d\n", rank);
    printArray(pProcArr, counts[rank]);
    printf("---------------\n");

    if (rank == root) {
        displs[size - 1] = 0;
        for (int i = 1; i < size; i++) {
            displs[size - 1 - i] = displs[size - i] + counts[size - i];
        }
        MPI_Bcast(displs, size, MPI_INT, root, MPI_COMM_WORLD);
    }

    MPI_Gatherv(pProcArr, counts[rank], MPI_INT, pResultArr, counts, displs, MPI_INT, root, MPI_COMM_WORLD);

    if (rank == root) {
        printf("Result array\n");
        printArray(pResultArr, length);
        printf("---------------\n");
    }

    MPI_Finalize();
    return 0;
}
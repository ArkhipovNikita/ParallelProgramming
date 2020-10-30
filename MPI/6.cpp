#include <iostream>
#include <mpi.h>

void printArray(int* arr, int size) {
    for (int i = 0; i < size; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

int main(int argc, char **argv) {
    srand (time(NULL));
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int arr_x = 10, arr_y = 10;
    int m[arr_x * arr_y];
    int counts[size], displs[size];
    int min_t, max_t, localMax, localMin, globalMax, globalMin;

    if (rank == 0) {
        for (int i = 0; i < arr_x; i++) {
            for (int j = 0; j < arr_y; j++) {
                m[i * arr_y + j] = i * arr_y + j;
            }
        }
        int restRows = arr_x;
        counts[0] = arr_x / size;
        displs[0] = 0;
        for (int i = 1; i < size; i++) {
            restRows -= counts[i - 1];
            counts[i] = restRows / (size - i);
            displs[i] = displs[i - 1] + counts[i - 1];
        }
        printArray(counts, size);
        printArray(displs, size);
    }

    MPI_Bcast(counts, size, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(displs, size, MPI_INT, 0, MPI_COMM_WORLD);

    int localM[counts[rank]][arr_y];
    int localMins[counts[rank]];
    int localMaxs[counts[rank]];

    MPI_Scatterv(m, counts, displs, MPI_INT, localM, counts[rank] * arr_y, MPI_INT, 0, MPI_COMM_WORLD);

    for (int i = 0; i < counts[rank]; i++)
    {
        min_t = localM[i][0];
        max_t = localM[i][0];
        for (int j = 1; j < arr_y; j++) {
            if (min_t > localM[i][j])
                min_t = localM[i][j];
            if (max_t < localM[i][j])
                max_t = localM[i][j];
        }
        localMins[i] = min_t;
        localMaxs[i] = max_t;
    }

    localMax = localMins[0];
    localMin = localMaxs[0];
    for (int i = 1; i < counts[rank]; i++) {
        if (localMax < localMins[i])
            localMax = localMins[i];
        if (localMin > localMaxs[i])
            localMin = localMaxs[i];
    }


    MPI_Reduce(&localMax, &globalMax, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);
    MPI_Reduce(&localMin, &globalMin, 1, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Global Max (among mins) = %d\n", globalMax);
        printf("Global Min (amons maxs) = %d\n", globalMin);
    }

    MPI_Finalize();
    return 0;
}
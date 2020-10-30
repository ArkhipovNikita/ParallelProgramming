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

void Scatter(int* sendbuf, int sendcount, MPI_Datatype sendtype, int* &recvbuf, int recvcount,
             MPI_Datatype recvtype, int root, MPI_Comm comm, int size, int rank) {
    recvbuf = new int[recvcount];
    if (rank == root) {
        for (int i = 0; i < recvcount; i++)
            recvbuf[i] = sendbuf[sendcount * root + i];
        for (int proc = 0; proc < size; proc++) {
            if (proc != root) {
                MPI_Send(&sendbuf[sendcount * proc], sendcount, sendtype, proc, proc, comm);
            }
        }
    } else {
        MPI_Status status;
        MPI_Recv(recvbuf, recvcount, recvtype, root, rank, comm, &status);
    }
}

void Gather(int* sendbuf, int sendcount, MPI_Datatype sendtype, int* &recvbuf, int recvcount,
            MPI_Datatype recvtype, int root, MPI_Comm comm, int size, int rank) {
    if (rank == root) {
        for (int i = 0; i < recvcount; i++)
            recvbuf[recvcount * root + i] = sendbuf[i];
        MPI_Status status;
        for (int proc = 0; proc < size; proc++) {
            if (proc != root) {
                MPI_Recv(&recvbuf[recvcount * proc], recvcount, recvtype, proc, proc, comm, &status);
            }
        }
    } else {
        MPI_Send(sendbuf, sendcount, sendtype, root, rank, comm);
    }
}

int main(int argc, char **argv) {
    int *pArr, *pProcArr, *pGatheredArr;
    int rank, size, procLength;
    int length = 10;
    int root = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    procLength = length / size;

    if (rank == root) {
        pArr = new int[length];
        pGatheredArr = new int[length];
        RandomDataInitialization(pArr, length, 100);
        printf("Initial array\n");
        printArray(pArr, length);
        printf("---------------\n");
    }

    Scatter(pArr, procLength, MPI_INT, pProcArr, procLength, MPI_INT, root, MPI_COMM_WORLD, size, rank);
    printf("Subarray of process №%d\n", rank);
    printArray(pProcArr, procLength);
    printf("---------------\n");

    Gather(pProcArr, procLength, MPI_INT, pGatheredArr, procLength, MPI_INT, root, MPI_COMM_WORLD, size, rank);
    if (rank == root) {
        printf("Gathered array\n");
        printArray(pGatheredArr, length);
        printf("---------------\n");
    }

    MPI_Finalize();
    return 0;
}
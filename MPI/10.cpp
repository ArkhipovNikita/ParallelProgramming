#include <iostream>
#include <mpi.h>
#include <ctime>
using namespace std;

void RandomDataInitialization (int* array, const int length, const int rand_diapason) {
    srand (time(NULL));
    for (int i = 0; i < length; i++)
        array[i] =  rand() % (2 * rand_diapason) - rand_diapason;
}


int main(int argc, char **argv) {
    int rank, size;
    int length = 1000;
    int pArr[length];
    int *pProcArr;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    RandomDataInitialization(pArr, length, 100);


    pProcArr = new int[length];
    auto start = chrono::system_clock::now();
    if (rank == 0) {
        MPI_Send(pArr, length, MPI_INT, 1, 1, MPI_COMM_WORLD);
        MPI_Recv(pProcArr, length, MPI_INT, 1, 2, MPI_COMM_WORLD, &status);
    } else {
        MPI_Recv(pProcArr, length, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
        MPI_Send(pArr, length, MPI_INT, 0, 2, MPI_COMM_WORLD);
    }
    auto end = chrono::system_clock::now();
    chrono::duration<double> elapsed = end - start;
    if (rank == 0)
        printf("Send: %f\n", elapsed.count());


    pProcArr = new int[length];
    start = chrono::system_clock::now();
    if (rank == 0) {
        MPI_Ssend(pArr, length, MPI_INT, 1, 3, MPI_COMM_WORLD);
        MPI_Recv(pProcArr, length, MPI_INT, 1, 4, MPI_COMM_WORLD, &status);
    } else {
        MPI_Recv(pProcArr, length, MPI_INT, 0, 3, MPI_COMM_WORLD, &status);
        MPI_Ssend(pArr, length, MPI_INT, 0, 4, MPI_COMM_WORLD);
    }
    end = chrono::system_clock::now();
    elapsed = end - start;
    if (rank == 0)
        printf("Ssend: %f\n", elapsed.count());


    pProcArr = new int[length];
    int buffer_attached_size = MPI_BSEND_OVERHEAD + length * sizeof(int);
    int* buffer_attached = (int*)malloc(buffer_attached_size);
    MPI_Buffer_attach(buffer_attached, buffer_attached_size);
    start = chrono::system_clock::now();
    if (rank == 0) {
        MPI_Bsend(pArr, length, MPI_INT, 1, 5, MPI_COMM_WORLD);
        MPI_Recv(pProcArr, length, MPI_INT, 1, 6, MPI_COMM_WORLD, &status);
    } else {
        MPI_Recv(pProcArr, length, MPI_INT, 0, 5, MPI_COMM_WORLD, &status);
        MPI_Bsend(pArr, length, MPI_INT, 0, 6, MPI_COMM_WORLD);
    }
    end = chrono::system_clock::now();
    elapsed = end - start;
    if (rank == 0)
        printf("Bsend: %f\n", elapsed.count());


    pProcArr = new int[length];
    start = chrono::system_clock::now();
    if (rank == 0) {
        MPI_Rsend(pArr, length, MPI_INT, 1, 7, MPI_COMM_WORLD);
        MPI_Recv(pProcArr, length, MPI_INT, 1, 8, MPI_COMM_WORLD, &status);
    } else {
        MPI_Recv(pProcArr, length, MPI_INT, 0, 7, MPI_COMM_WORLD, &status);
        MPI_Rsend(pArr, length, MPI_INT, 0, 8, MPI_COMM_WORLD);
    }
    end = chrono::system_clock::now();
    elapsed = end - start;
    if (rank == 0)
        printf("Rsend: %f\n", elapsed.count());


    MPI_Finalize();
    return 0;
}
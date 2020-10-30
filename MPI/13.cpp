#include <iostream>
#include <mpi.h>

void RandomDataInitialization (int* array, const int length, const int diapason) {
    srand (time(NULL));
    for (int i = 0; i < length; i++)
        array[i] = i; // rand() % (2 * diapason) - diapason
}

void printArray(bool* arr, int size) {
    for (int i = 0; i < size; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

void print2dArray(int *arr, int x, int y) {
    for (int i = 0; i < x; i ++) {
        for (int j = 0; j < y; j++)
            printf("%d ", arr[i * y + j]);
        printf("\n");
    }
}

int main(int argc, char **argv) {
    int rank, size;
    int *pProcRows, *pProcColumns;
    bool *pResult;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int m_size = 3;
    int pMatrix[9] = {1, 3, 0, 3, 2, 6, 0, 1, 5};
    pResult = new bool[size];

    MPI_Datatype col, colType;
    MPI_Type_vector(m_size, 1, m_size, MPI_INT, &col);
    MPI_Type_commit(&col);
    // что это делает?
    MPI_Type_create_resized(col, 0,1 * sizeof(int), &colType);
    MPI_Type_commit(&colType);

    if (rank == 0) {
//        RandomDataInitialization(pMatrix, m_size * m_size, 100);
//        pMatrix = {1, 3, 0, 3, 2, 6, 0, 6, 5};
        printf("Изначальная матрица\n");
        print2dArray(pMatrix, m_size, m_size);
        printf("--------------\n");
    }

    int lineNum = m_size / size;
    int count = m_size * lineNum;
    pProcRows = new int[count];
    pProcColumns = new int[count];

    MPI_Scatter(&pMatrix[rank * lineNum], count, MPI_INT, pProcRows, count, MPI_INT, 0 , MPI_COMM_WORLD);
    MPI_Scatter(&pMatrix[rank * lineNum], lineNum, colType, pProcColumns, count, MPI_INT, 0, MPI_COMM_WORLD);

    bool res = true;
    for (int i = 0; i < count; i++) {
        if (pProcRows[i] != pProcColumns[i]) {
            res = false;
            break;
        }
    }

    MPI_Gather(&res, 1, MPI_C_BOOL, pResult, 1, MPI_C_BOOL, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        for (int i = 0; i < size; i++) {
            if (!pResult[i]) {
                printf("Матрица несимметрична\n");
                break;
            }
        }
        printf("Матрица симметрична\n");
    }

    MPI_Finalize();
    return 0;
}
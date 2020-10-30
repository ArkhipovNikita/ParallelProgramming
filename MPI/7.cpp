#include <iostream>
#include <mpi.h>

void printArray(int* arr, int size) {
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

void randomDataInitialization (int* array, const int length, const int diapason) {
    srand (time(NULL));
    for (int i = 0; i < length; i++)
        array[i] = i; // rand() % (2 * diapason) - diapason
}

int main(int argc, char* argv[]) {
    int m_x = 3;
    int m_y = 4;
    int *pMatrix, *pVector, *pResult, *pProcColumns, *pProcVector, *pProcResult;
    int rank, size, columnsNum;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    columnsNum = m_y / size;
    pProcColumns = new int[columnsNum * m_x];
    pProcVector = new int[columnsNum];
    pProcResult = new int[columnsNum * m_x];

    if (rank == 0) {
        pMatrix =  new int [m_x * m_y];
        pVector = new int [m_y];
        pResult = new int [m_x];
        RandomDataInitialization(pMatrix, m_x * m_y, 100);
        RandomDataInitialization(pVector, m_y, 100);
        printf("Изначальная матрица\n");
        print2dArray(pMatrix, m_x, m_y);
        printf("--------------\n");
    }

    MPI_Datatype col, coltype;
    MPI_Type_vector(m_x, 1, m_y, MPI_INT, &col);
    MPI_Type_commit(&col);
    MPI_Type_create_resized(col, 0,1 * sizeof(int), &coltype);
    MPI_Type_commit(&coltype);


    MPI_Status status;
    if (rank == 0) {
        for (int j = 0; j < columnsNum; j++) {
            for (int i = 0; i < m_x; i++) {
                pProcColumns[j * m_x + i] = pMatrix[j + m_y * i];
            }
        }
        for (int p = 1; p < size; p++) {
            for (int j = 0; j < columnsNum; j++) {
                MPI_Send(&pMatrix[p * columnsNum + j], 1, col, p, p, MPI_COMM_WORLD);
            }
        }
    } else {
        for (int j = 0; j < columnsNum; j++) {
            MPI_Recv(&pProcColumns[j * m_x], m_x, MPI_INT, 0, rank, MPI_COMM_WORLD, &status);
        }
    }

//    MPI_Scatterv(pMatrix, counts, displs, coltype, pProcColumns,columnsNum * m_x, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(pVector, columnsNum, MPI_INT, pProcVector, columnsNum, MPI_INT, 0, MPI_COMM_WORLD);

//    printf("Матрица в процессе\n");
//    print2dArray(pProcColumns, columnsNum, m_x);
//    printf("--------------\n");
//    printf("Вектор в процессе\n");
//    printArray(pProcVector, columnsNum);

    for (int j = 0; j < columnsNum; j++) {
        for (int i = 0; i < m_x; i++) {
            pProcResult[j * m_x + i] = pProcColumns[j * m_x + i] * pProcVector[j];
        }
    }

    MPI_Gather(pProcResult, columnsNum * m_x, MPI_INT, &pMatrix[rank * columnsNum * m_x], columnsNum * m_x, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Result matrix\n");
        print2dArray(pMatrix, m_y, m_x);
        printf("--------------\n");
    }

    if (rank == 0) {
        for (int i = 0; i < m_x; i++) {
            pResult[i] = 0;
            for (int j = 0; j < m_y; j++) {
                pResult[i] += pMatrix[i + j * m_x];
            }
            printf("%d\n", pResult[i]);
        }
    }

    MPI_Finalize();
    return  0;
}
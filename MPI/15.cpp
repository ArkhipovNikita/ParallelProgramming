#include <iostream>
#include <mpi.h>
#include <cmath>

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

void randomDataInitialization(int* array, const int length, const int diapason) {
    srand (time(NULL));
    for (int i = 0; i < length; i++) {
//        array[i] =  rand() % (2 * diapason) - diapason;
        array[i] = i;
    }
}

void fillArrayWith(int* array, const int length, int n) {
    for (int i = 0; i < length; i++) {
        array[i] = n;
    }
}

void dataDistribution(int *pAMatrix, int *pBMatrix, int *pCMatrix, int *pABlock, int *pBBlock, int *pCBlock, MPI_Datatype blockType,
                      int matrixSize, int blockSize, int gridSize, int rank, MPI_Comm gridComm) {
    if (rank == 0) {
        for (int i = 0; i < blockSize; i++) {
            for (int j = 0; j < blockSize; j++) {
                pABlock[i * blockSize + j] = pAMatrix[i * matrixSize + j];
                pBBlock[i * blockSize + j] = pBMatrix[i * matrixSize + j];
                pCBlock[i * blockSize + j] = pCMatrix[i * matrixSize + j];
            }
        }
    }
    bool flag = false;
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            int curCartRank;
            int send_cords[2] = {i, j};
            MPI_Cart_rank(gridComm, send_cords, &curCartRank);
            if (rank == 0) {
                if (flag) {
                    MPI_Send(&pAMatrix[i * matrixSize * blockSize + j * blockSize], 1, blockType, curCartRank, curCartRank, MPI_COMM_WORLD);
                    MPI_Send(&pBMatrix[i * matrixSize * blockSize + j * blockSize], 1, blockType, curCartRank, curCartRank, MPI_COMM_WORLD);
                    MPI_Send(&pCMatrix[i * matrixSize * blockSize + j * blockSize], 1, blockType, curCartRank, curCartRank, MPI_COMM_WORLD);
                }
                flag = true;
            } else if (rank == curCartRank) {
                MPI_Recv(pABlock, blockSize * blockSize, MPI_INT, 0, rank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(pBBlock, blockSize * blockSize, MPI_INT, 0, rank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(pCBlock, blockSize * blockSize, MPI_INT, 0, rank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
        }
    }
}

void resultCollection(int *pCMatrix, int *pCBlock, MPI_Datatype blockType,
                      int matrixSize, int blockSize, int gridSize, int rank, MPI_Comm gridComm) {
    if (rank != 0) {
        MPI_Send(pCBlock, blockSize * blockSize, MPI_INT, 0, rank, MPI_COMM_WORLD);
    } else {
        bool flag = false;
        for (int i = 0; i < blockSize; i++) {
            for (int j = 0; j < blockSize; j++) {
                pCMatrix[i * matrixSize + j] = pCBlock[i * blockSize + j];
            }
        }
        for (int i = 0; i < gridSize; i++) {
            for (int j = 0; j < gridSize; j++) {
                if (flag) {
                    int sourceRank = i * gridSize + j;
                    MPI_Recv(&pCMatrix[i * blockSize * matrixSize + j * blockSize], 1, blockType, sourceRank, sourceRank, MPI_COMM_WORLD,
                             MPI_STATUS_IGNORE);
                }
                flag = true;
            }
        }
    }
}

void broadcastA(int i, int cords[2], int gridSize, MPI_Comm rowComm, int *pABlock, int *pABlockBuf, int m) {
    int k = (cords[0] + i) % gridSize;
    if (cords[1] == k) {
        for (int j = 0; j < m * m; j++)
            pABlockBuf[j] = pABlock[j];
    }
    MPI_Bcast(pABlockBuf, m * m, MPI_INT, k, rowComm);
}

void blockMultiplication(int blockSize, int *a, int *b, int *c) {
    for (int i = 0; i < blockSize; i++) {
        for (int j = 0; j < blockSize; j++) {
            int t = 0;
            for (int k = 0; k < blockSize; k++) {
                t += a[i * blockSize + k] * b[j + k * blockSize];
            }
            c[i * blockSize + j] += t;
        }
    }
}

void shiftBBlock(int cords[2], int gridSize, int *pBBlock, int m, MPI_Comm colComm) {
    int prevProc = (cords[0] + 1) % gridSize;
    int nextProc = cords[0] == 0 ? gridSize - 1 : cords[0] - 1;
    MPI_Sendrecv_replace(pBBlock, m * m, MPI_INT, nextProc, 0, prevProc, 0, colComm, MPI_STATUS_IGNORE);
}

int main(int argc, char **argv) {
    // http://www.cas.mcmaster.ca/~nedialk/COURSES/mpi/Lectures/lec3_3.pdf
    // https://intuit.ru/studies/courses/1156/190/lecture/4954?page=4
    int rank, size;
    int matrixSize = 6;
    int pAMatrix[matrixSize * matrixSize], pBMatrix[matrixSize * matrixSize], pCMatrix[matrixSize * matrixSize];
    int gridCords[2];
    MPI_Comm gridComm;
    MPI_Comm rowComm;
    MPI_Comm colComm;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int gridSize = 2;
    int blockSize = matrixSize / gridSize;
    int pABlock[blockSize * blockSize], pABlockBuf[blockSize * blockSize], pBBlock[blockSize * blockSize], pCBlock[blockSize * blockSize];

    int dims[2];
    int periods[2];
    for (int i = 0; i < 2; i++) {
        dims[i] = gridSize;
        periods[i] = 0;
    }

    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 1, &gridComm);
    MPI_Cart_coords(gridComm, rank, 2, gridCords);

    int subDims[2];
    subDims[0] = 0;
    subDims[1] = 1;
    MPI_Cart_sub(gridComm, subDims, &rowComm);
    subDims[0] = 1;
    subDims[1] = 0;
    MPI_Cart_sub(gridComm, subDims, &colComm);

    MPI_Datatype blockType;
    MPI_Type_vector(blockSize, blockSize, matrixSize, MPI_INT, &blockType);
    MPI_Type_commit(&blockType);

    if (rank == 0) {
        randomDataInitialization(pAMatrix, matrixSize * matrixSize, 10);
        randomDataInitialization(pBMatrix, matrixSize * matrixSize, 10);
        fillArrayWith(pCMatrix, matrixSize * matrixSize, 0);
        printf("Изначальная матрица A\n");
        print2dArray(pAMatrix, matrixSize, matrixSize);
        printf("--------------\n");
//        printf("Изначальная матрица B\n");
//        print2dArray(pBMatrix, matrixSize, matrixSize);
//        printf("--------------\n");
//        printf("Изначальная матрица C\n");
//        print2dArray(pCMatrix, matrixSize, matrixSize);
//        printf("--------------\n");
    }

    dataDistribution(pAMatrix, pBMatrix, pCMatrix, pABlock, pBBlock, pCBlock,
                     blockType, matrixSize, blockSize, gridSize, rank, gridComm);

//    printf("Rank = %d, Cart_rank[0] = %d, Cart_rank[1] = %d\n", rank, gridCords[0], gridCords[1]);
//    printf("Локальная матрица C\n");
//    print2dArray(pCBlock, blockSize, blockSize);
//    printf("--------------\n\n\n");

    for (int i = 0; i < gridSize; i++) {
        broadcastA(i, gridCords, gridSize, rowComm, pABlock, pABlockBuf, blockSize);
        blockMultiplication(blockSize, pABlockBuf, pBBlock, pCBlock);
        shiftBBlock(gridCords, gridSize, pBBlock, blockSize, colComm);
    }

//    for (int i = 0; i<blockSize*blockSize; i++) {
//        pCBlock[i] = rank + i;
//    }

//    for (int i = 0; i < size; i++) {
//        if (rank == i) {
//            printf("Rank = %d, Cart_rank[0] = %d, Cart_rank[1] = %d\n", rank, gridCords[0], gridCords[1]);
//            printf("Локальная матрица A\n");
//            print2dArray(pABlock, blockSize, blockSize);
//            printf("--------------\n");
//            printf("Локальная матрица B\matrixSize");
//            print2dArray(pBBlock, blockSize, blockSize);
//            printf("--------------\matrixSize\matrixSize\matrixSize");
//            printf("Локальная матрица C\n");
//            print2dArray(pCBlock, blockSize, blockSize);
//            printf("--------------\n\n\n");
//        }
//    }

//    int rowRank, colRank;
//    MPI_Comm_rank(rowComm, &rowRank);
//    MPI_Comm_rank(colComm, &colRank);
//    printf("Rank = %d, Cart_rank[0] = %d, Cart_rank[1] = %d, RowRank = %d, ColRank = %d\matrixSize", rank, gridCords[0], gridCords[1], rowRank, colRank);

    resultCollection(pCMatrix, pCBlock, blockType, matrixSize, blockSize, gridSize, rank, gridComm);

    if (rank == 0)
        print2dArray(pCMatrix, matrixSize, matrixSize);

    MPI_Finalize();
    return 0;
}
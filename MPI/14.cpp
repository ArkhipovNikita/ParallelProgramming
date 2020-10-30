#include <iostream>
#include <mpi.h>

void randomDataInitialization(int* array, const int length, const int diapason) {
    srand (time(NULL));
    for (int i = 0; i < length; i++)
        array[i] =  rand() % (2 * diapason) - diapason;
}

void printArray(int* arr, int size) {
    for (int i = 0; i < size; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

void merge(int* arr, int left, int middle, int right) {
    int *first = &arr[left];
    int n1 = middle - left + 1;
    int *second = &arr[middle + 1];
    int n2 = right - middle;
    int res[right - left + 1];

    int i = 0, j = 0, k = 0;
    while (i < n1 and j < n2) {
        if (first[i] < second[j]) {
            res[k] = first[i];
            i++;
        }
        else {
            res[k] = second[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        res[k] = first[i];
        k++;
        i++;
    }

    while (j < n2) {
        res[k] = second[j];
        k++;
        j++;
    }

    for (int q = 0; q < (right - left + 1); q++)
        arr[left + q] = res[q];
}

void merge_sort(int* arr, int left, int right) {
    if (left < right) {
        int middle = left + (right - left) / 2;
        merge_sort(arr, left, middle);
        merge_sort(arr, middle + 1, right);
        merge(arr, left, middle, right);
    }
}

void compare_split(int *pProcArr, int send_n, int rcv_n, int sendRank, int receiveRank) {
    int mergedArr[send_n + rcv_n];
    int rank;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == sendRank) {
        MPI_Send(pProcArr, send_n, MPI_INT, receiveRank, receiveRank, MPI_COMM_WORLD);
        MPI_Recv(pProcArr, send_n, MPI_INT, receiveRank, sendRank, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
    } else if (rank == receiveRank) {
        MPI_Recv(mergedArr, send_n, MPI_INT, sendRank, receiveRank, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);

        for (int i = 0; i < rcv_n; i++)
            mergedArr[send_n + i] = pProcArr[i];

        merge(mergedArr, 0, ((send_n + rcv_n) - 1) / 2, (send_n + rcv_n) - 1);

        int mystart = sendRank > receiveRank ? 0 : send_n;

        for (int i = 0; i < rcv_n; i++)
            pProcArr[i] = mergedArr[mystart + i];

        MPI_Send(&mergedArr[send_n - mystart], send_n, MPI_INT, sendRank, sendRank, MPI_COMM_WORLD);
    }

}

int main(int argc, char* argv[]) {
    int rank, size;
    int n = 12;
    int *pArr, *pProcArr;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        pArr = new int[n];
        randomDataInitialization(pArr, n, 10);
        printf("Изначальный массив\n");
        printArray(pArr, n);
        printf("--------------\n");
    }

    int counts[size], displs[size];
    int restElems = n;
    counts[0] = n / size;
    displs[0] = 0;
    for (int i = 1; i < size; i++) {
        restElems -= counts[i - 1];
        counts[i] = restElems / (size - i);
        displs[i] = displs[i - 1] + counts[i - 1];
    }
    pProcArr = new int[counts[rank]];

    MPI_Scatterv(pArr, counts, displs, MPI_INT, pProcArr, counts[rank], MPI_INT, 0, MPI_COMM_WORLD);

    merge_sort(pProcArr, 0, counts[rank] - 1);

    for (int i = 0; i < size; i++) {
        if ((i + rank) % 2 == 0) {
            if (rank < size - 1) compare_split(pProcArr, counts[rank], counts[rank + 1], rank, rank + 1);
        } else if (rank > 0) {
            compare_split(pProcArr, counts[rank - 1], counts[rank], rank - 1, rank);
        }
    }

    MPI_Gatherv(pProcArr, counts[rank], MPI_INT, pArr, counts, displs, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Отсортированный массив\n");
        printArray(pArr, n);
        printf("--------------\n");
    }

    return 0;
}

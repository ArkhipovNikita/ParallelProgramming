#include <iostream>
#include <mpi.h>


void compare_exchange(int *p1, int *p2) {
    if (*p1 > *p2) {
        int t = *p2;
        *p2 = *p1;
        *p1 = t;
        printf("Смена %d %d\n", *p1, *p2);
    }
}

void printArray(int* arr, int size) {
    for (int i = 0; i < size; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

void OddEvenSort1(int A[], int n) {
    for (int i = 1; i < n; i++) {
        if (i % 2 == 1) {    // нечетная итерация
            for (int j = 0; j < n/2; j++)
                compare_exchange(&A[2*j + 1], &A[2*j + 2]);
            if (n % 2 == 1) // сравнение последней пары при нечетном n
                compare_exchange(&A[n - 2], &A[n - 1]);
        }
        else    // четная итерация
            for (int j = 0; j < n/2; j++)
                compare_exchange(&A[2*j], &A[2*j + 1]);
    }
}

void OddEvenSort(int *A, int n) {
    int even_n = n % 2 == 0 ? n / 2 - 1 : n  / 2;
    int odd_n = n / 2;
    for (int i = 0; i < n; i++) {
        if (i % 2 == 1) {
             for (int j = 0; j < odd_n; j++)
                compare_exchange(&A[2 * j], &A[2 * j + 1]);
        } else {
            for (int j = 0; j < even_n; j++)
                compare_exchange(&A[2 * j + 1], &A[2 * j + 2]);
        }
    }
}

int main(int argc, char* argv[]) {
    int n = 8;
    int a[8] = {9, 5, 1, 0, -1, -2, 3, 8};
    OddEvenSort(a, n);
    printArray(a, n);
    return 0;
}
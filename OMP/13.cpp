#include <iostream>
#include <omp.h>
#include <thread>
#include <chrono>

void first() {
#pragma omp parallel num_threads(8)
    {
        int n = omp_get_num_threads();
        for (int i = n - 1; i >= 0; i--)
        {
#pragma omp barrier
            {
                if (i == omp_get_thread_num())
                    printf("%d thread of %d threads, Hello World!\n", omp_get_thread_num(), omp_get_num_threads());
            }
        }
    }
}

void second() {
#pragma omp parallel num_threads(8)
    {
        int n = omp_get_num_threads();
        std::this_thread::sleep_for (std::chrono::seconds(7 - omp_get_thread_num()));
        printf("%d thread of %d threads, Hello World!\n", omp_get_thread_num(), omp_get_num_threads());
    }
}

void third() {
    int i;
#pragma parallel for num_threads(8) schedule(static, 2) shared(i)
    for (i = 7; i >= 0; i--)
    {
        printf("i = %d; thread = %d\n", i, omp_get_thread_num());
        if (i == omp_get_thread_num()) {
            printf("%d thread of %d threads, Hello World!\n", omp_get_thread_num(), omp_get_num_threads());
        } else
            i++;
    }
}

int main() {
//    first();
    second();
//    third();
    return 0;
}

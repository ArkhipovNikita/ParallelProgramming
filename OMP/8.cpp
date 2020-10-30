#include <iostream>
#include <omp.h>
#include <ctime>


int main() {
    int a[1600];
    int b[1600];

    for (int i = 0; i < sizeof(a) / sizeof(a[0]); i++)
        a[i] = i;

    unsigned int start_time;
    unsigned int end_time;

    for (int i = 1; i <= 11; i += 2) {
        start_time = clock();
#pragma omp parallel for num_threads(8) schedule(static, i)
        for (int i = 1; i < sizeof(a) / sizeof(a[0]) - 1; i++)
            b[i] = (a[i - 1] + a[i] + a[i + 1]) / 3;
        end_time = clock();
        printf("type: static; chunk: %d; time:  %d milliseconds\n", i, end_time - start_time);
    }

    for (int i = 1; i <= 11; i += 2) {
        start_time = clock();
#pragma omp parallel for num_threads(8) schedule(dynamic)
        for (int i = 1; i < sizeof(a) / sizeof(a[0]) - 1; i++)
            b[i] = (a[i - 1] + a[i] + a[i + 1]) / 3;
        end_time = clock();
        printf("type: dynamic; chunk: %d; time:  %d milliseconds\n", i, end_time - start_time);
    }

    for (int i = 1; i <= 11; i += 2) {
        start_time = clock();
#pragma omp parallel for num_threads(8) schedule(guided)
        for (int i = 1; i < sizeof(a) / sizeof(a[0]) - 1; i++)
            b[i] = (a[i - 1] + a[i] + a[i + 1]) / 3;
        end_time = clock();
        printf("type: guided; chunk: %d; time:  %d milliseconds\n", i, end_time - start_time);
    }

    for (int i = 1; i <= 11; i += 2)
    {
        start_time = clock();
#pragma omp parallel for num_threads(8) schedule(auto)
    for (int i = 1; i < sizeof(a) / sizeof(a[0]) - 1; i++)
        b[i] = (a[i - 1] + a[i] + a[i + 1]) / 3;
    end_time = clock();
    printf("type: auto; chunk: %d; time:  %d milliseconds\n", i, end_time - start_time);
    }

    return 0;
}

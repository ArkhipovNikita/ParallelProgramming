#include <iostream>
#include <omp.h>


int main() {
    int a[3000];

    srand (time(NULL));
    for (int i = 0; i < 30; i++)
        a[i] = i;


    int max = -2147483648;
#pragma omp parallel num_threads(8) shared(a, max)
    {
        int localMax = max;
#pragma omp for nowait
        for (int i = 0; i < 30; i++)
        {
            printf("%d thread of %d threads\n", omp_get_thread_num(), omp_get_num_threads());
            if (a[i] % 7 == 0 & a[i] > localMax)
                localMax = a[i];
        }

#pragma omp critical
        {
            if (localMax > max)
                max = localMax;
        }
    }

    printf("Max = %d\n", max);

    return 0;
}
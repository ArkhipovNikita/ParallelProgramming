#include <iostream>
#include <omp.h>


int main() {
    int d[6][8];

    for (int i = 0; i < 6; i++)
        for (int j = 0; j < 8; j++)
            d[i][j] = i * 8 + j;

    int min = 2147483647;
    int max = -2147483648;
#pragma omp parallel num_threads(8) shared(min, max, d)
    {
        int localMax = max;
        int localMin = min;
        printf("%d\n", localMax);
#pragma omp for nowait
        for (int i = 0; i < 6; i++)
            for (int j = 0; j < 8; j++)
            {
//                printf("%d thread of %d threads\n", omp_get_thread_num(), omp_get_num_threads());
                if (d[i][j] < localMin)
                    localMin = d[i][j];
                else if (d[i][j] > localMax)
                    localMax = d[i][j];
            }
#pragma omp critical
        {
            if (localMax > max)
                max = localMax;
            if (localMin < min)
                min = localMin;
        };
    }

    printf("Min = %d\n", min);
    printf("Max = %d\n", max);

    return 0;
}

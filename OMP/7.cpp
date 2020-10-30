#include <iostream>
#include <omp.h>


int main() {
    int a[12];
    int b[12];
    int c[12];

#pragma omp parallel for num_threads(3) schedule(static, 3)
        for (int i = 0; i < 12; i++)
        {
            a[i] = i * 2;
            b[i] = i * 3;
            printf("%d thread of %d threads; a[%d] = %d; b[%d] = %d\n",
                   omp_get_thread_num(), omp_get_num_threads(), i, a[i], i, b[i]);
        }

#pragma omp parallel for num_threads(4) schedule(dynamic, 2)
    for (int i = 0; i < 12; i++)
    {
        c[i] = a[i] + b[i];
        printf("%d thread of %d threads; c[%d] = %d\n",
               omp_get_thread_num(), omp_get_num_threads(), i, c[i]);
    }
    return 0;
}

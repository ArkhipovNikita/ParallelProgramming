#include <iostream>
#include <omp.h>


int main() {
    int a[30];

    srand (time(NULL));
    for (int i = 0; i < 30; i++)
            a[i] = rand() % (100001);


    int r = 0;
#pragma omp parallel for num_threads(4) shared(a, r)
    for (int i = 0; i < 30; i++)
        {
            printf("%d thread of %d threads\n", omp_get_thread_num(), omp_get_num_threads());
            if (a[i] % 9 == 0)
#pragma omp atomic
                r++;
        }
    printf("Res = %d\n", r);
    for (int i = 0; i < 30; i++)
        printf("%d ", a[i]);

    return 0;
}

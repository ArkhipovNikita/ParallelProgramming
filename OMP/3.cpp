#include <iostream>
#include <omp.h>


int main() {
    int a = 1;
    int b = 2;

    printf("before 1st block:  a = %d; b = %d\n", a, b);

#pragma omp parallel num_threads(2) private(a) firstprivate(b)
    {
        int n_thread = omp_get_thread_num();
        a += n_thread;
        b += n_thread;
        printf("in 1st block: a = %d; b = %d; %d thread of %d threads\n", a, b, n_thread, omp_get_num_threads());
    }

    printf("after 1st block:  a = %d; b = %d\n", a, b);

    printf("before 2st block:  a = %d; b = %d\n", a, b);

#pragma omp parallel num_threads(4) shared(a) private(b)
    {
        int n_thread = omp_get_thread_num();
        a -= n_thread;
        b -= n_thread;
        printf("in 2st block: a = %d; b = %d; %d thread of %d threads\n", a, b, n_thread, omp_get_num_threads());
    }

    printf("after 2st block:  a = %d; b = %d\n", a, b);
    return 0;
}

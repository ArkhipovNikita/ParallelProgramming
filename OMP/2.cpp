#include <iostream>
#include <omp.h>


int main() {
    int threads_num = 3;
#pragma omp parallel if(threads_num > 2)
    {
        printf("1st block %d thread of %d threads\n", omp_get_thread_num(), omp_get_num_threads());
    }

    threads_num = 2;
#pragma omp parallel if(threads_num > 2)
    {
        printf("2nd block %d thread of %d threads\n", omp_get_thread_num(), omp_get_num_threads());
    }
    return 0;
}

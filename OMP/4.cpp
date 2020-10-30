#include <iostream>
#include <omp.h>


int main() {
    int a[10];
    int b[10];
    for (int i = 0; i < sizeof(a) / sizeof(a[0]); i++)
    {
        a[i] = i * 2;
        b[i] = i * 3;
    }

#pragma omp parallel num_threads(2)
    {
        if (omp_get_thread_num() == 0)
        {
            int min = 2147483647;
            for (int i = 0; i < sizeof(a) / sizeof(a[0]); i++)
            {
                if (a[i] < min)
                    min = a[i];
            }
            printf("Min = %d\n", min);
        }
        else
        {
            int max = -2147483648;
            for (int i = 0; i < sizeof(b) / sizeof(b[0]); i++)
            {
                if (b[i] > max)
                    max = b[i];
            }
            printf("Max = %d\n", max);
        }
    }
    return 0;
}

#include <iostream>
#include <omp.h>


int main() {
    int a[100];
    for (int i = 0; i < sizeof(a) / sizeof(a[0]); i++)
        a[i] = i * 2;


    int m1 = 0;
#pragma omp parallel for reduction(+:m1)
    for (int i = 0; i < sizeof(a) / sizeof(a[0]); i++)
        m1 += a[i];
    m1 /= sizeof(a) / sizeof(a[0]);

    int m2 = 0;
#pragma omp parallel for
    for (int i = 0; i < sizeof(a) / sizeof(a[0]); i++)
        m2 += a[i];
    m2 /= sizeof(a) / sizeof(a[0]);

    printf("m1 = %d; m2 = %d", m1, m2);
    return 0;
}

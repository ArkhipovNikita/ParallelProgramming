#include <iostream>
#include <omp.h>


int main() {
    int cols = 8;
    int rows = 6;
    int d[6][8];
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
        {
            d[i][j] = rand() / 100;
        }


#pragma omp parallel sections
    {
#pragma omp section
        {
            int res;
            for (int i = 0; i < rows; i++)
                for (int j = 0; j < cols; j++)
                {
                    res += d[i][j];
                }
            res /= rows * cols;
            printf("Среднее арифметическое = %d\n", res);
        }
#pragma omp section
        {
            int min = 2147483647;
            int max = -2147483648;
            for (int i = 0; i < rows; i++)
                for (int j = 0; j < cols; j++)
                {
                    if (d[i][j] < min)
                        min = d[i][j];
                    else if (d[i][j] > max)
                        max = d[i][j];
                }
            printf("Min = %d; Max = %d\n", min, max);
        }
#pragma omp section
        {
            int res = 0;
            for (int i = 0; i < rows; i++)
                for (int j = 0; j < cols; j++)
                {
                    if (d[i][j] % 3 == 0)
                        res++;
                }
            printf("Кол-во эл-в кратных 3-м %d\n", res);
        }
    }
    return 0;
}

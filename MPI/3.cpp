#include <iostream>
#include <mpi.h>
#include <cmath>

#define N 100000

// центр в начале координат
bool is_circle_point(double x, double y, double r)
{
    return abs(sqrt(x * x + y * y)) <= r;
}

int main(int argc, char **argv) {
    srand (time(NULL));
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    double x, y;
    int nCircle;
    int nCircleLocal = 0;
    int a = 5000;
    int r = a / 2;
    int ops_per_proc = N / size;

    for (int i = 0; i < ops_per_proc; i++)
    {
//        printf("Rank = %d\n", rank);
        x = (double) rand() / RAND_MAX * a - r;
        y = (double) rand() / RAND_MAX * a - r;
        if (is_circle_point(x, y, r))
            nCircleLocal++;
    }

    MPI_Reduce(&nCircleLocal, &nCircle, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        printf("nCircle = %d, nCommon = %d\n", nCircle, N);
        printf("Pi = %f\n", (nCircle / (N * 1.0)) * 4);
    }

    MPI_Finalize();
    return 0;
}
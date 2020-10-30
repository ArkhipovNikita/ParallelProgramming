#include <iostream>
#include <mpi.h>

void Loop(MPI_Comm comm, char comm_char, int rank, int size) {
    MPI_Status status;
    int num = 0;
    if (rank == 0)
        MPI_Send(&num, 1, MPI_INT, 1, 0, comm);
    int i = 1;
    while (true) {
        if (rank == i) {
            int prev_i = i == 0 ? size - 1 : i - 1;
            MPI_Recv(&num, 1, MPI_INT, prev_i, prev_i, comm, &status);
            printf("Comm: %c, Rank = %d, buf = %d\n", comm_char, rank, num);
            num = num + 1;
            MPI_Send(&num, 1, MPI_INT, (i + 1) % size, i, comm);
        }
        if (i == 0)
            break;
        i = (i + 1) % size;
    }
}

int main(int argc, char **argv) {
    int rank, size, num;
    const int limit = 20;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    Loop(MPI_COMM_WORLD, 'W', rank, size);

    int *evens_ranks;
    int n = size % 2 != 0 ? size / 2 + 1 : size / 2;
    evens_ranks = new int[n];
    int j = 0;
    for (int i = 0; i < size; i++) {
        if (i % 2 == 0) {
            evens_ranks[j] = i;
            j++;
        }
    }
    MPI_Group WorldGroup, EvensGroup;
    MPI_Comm Evens;
    MPI_Comm_group(MPI_COMM_WORLD, &WorldGroup);
    MPI_Group_incl(WorldGroup, n, evens_ranks, &EvensGroup);
    MPI_Comm_create(MPI_COMM_WORLD, EvensGroup, &Evens);

    if (Evens != MPI_COMM_NULL) {
        int n_rank, n_size;
        MPI_Comm_size(Evens, &n_size);
        MPI_Comm_rank(Evens, &n_rank);
        Loop(Evens, 'E', n_rank, n_size);
    }

    MPI_Finalize();
    return 0;
}
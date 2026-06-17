#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <omp.h>

#define N 1000 // Tamaño de las matrices

int main(int argc, char *argv[]) {
    int rank, size;
    double start_time, end_time;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (N % size != 0) {
        if (rank == 0) printf("Error: El tamaño N debe ser divisible por size.\n");
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    int rows_per_proc = N / size;
    double *A = NULL, *C = NULL;
    double *B = (double *)malloc(N * N * sizeof(double));
    double *local_A = (double *)malloc(rows_per_proc * N * sizeof(double));
    double *local_C = (double *)malloc(rows_per_proc * N * sizeof(double));

    if (rank == 0) {
        A = (double *)malloc(N * N * sizeof(double));
        C = (double *)malloc(N * N * sizeof(double));
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                A[i * N + j] = (double)(i + j);
                B[i * N + j] = (double)(i - j);
            }
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    if (rank == 0) start_time = MPI_Wtime();

    MPI_Scatter(A, rows_per_proc * N, MPI_DOUBLE, local_A, rows_per_proc * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(B, N * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    #pragma omp parallel
    {
        #pragma omp for
        for (int i = 0; i < rows_per_proc; i++) {
            for (int j = 0; j < N; j++) {
                double temp_sum = 0.0;
                for (int k = 0; k < N; k++) {
                    temp_sum += local_A[i * N + k] * B[k * N + j];
                }
                local_C[i * N + j] = temp_sum;
            }
        }
    }

    MPI_Gather(local_C, rows_per_proc * N, MPI_DOUBLE, C, rows_per_proc * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        end_time = MPI_Wtime();
        int num_threads = 0;
        #pragma omp parallel
        {
            #pragma omp single
            num_threads = omp_get_num_threads();
        }
        printf("==== Resultados ====\n");
        printf("Matriz N x N : %d x %d\n", N, N);
        printf("Procesos MPI : %d\n", size);
        printf("Hilos OpenMP : %d\n", num_threads);
        printf("Tiempo       : %f segundos\n", end_time - start_time);
        free(A); free(C);
    }

    free(B); free(local_A); free(local_C);
    MPI_Finalize();
    return EXIT_SUCCESS;
}

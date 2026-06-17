#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define N 1024 

void print_matrix_preview(double *mat, int rows, int cols, const char* name) {
    printf("Previsualizacion de %s:\n", name);
    for (int i = 0; i < (rows < 3 ? rows : 3); i++) {
        for (int j = 0; j < (cols < 3 ? cols : 3); j++) {
            printf("%6.2f ", mat[i * cols + j]);
        }
        printf("...\n");
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    int rank, size;
    double inicio_tiempo, fin_tiempo;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (N % size != 0) {
        if (rank == 0) printf("Error: N (%d) debe ser divisible por procesos (%d).\n", N, size);
        MPI_Finalize();
        return 1;
    }

    int filas_por_proceso = N / size;
    int hilos_omp = 4; // Ajustable
    omp_set_num_threads(hilos_omp);

    double *A = NULL, *B = NULL, *C = NULL;
    double *local_A = (double *)malloc(filas_por_proceso * N * sizeof(double));
    double *local_C = (double *)malloc(filas_por_proceso * N * sizeof(double));
    B = (double *)malloc(N * N * sizeof(double));

    if (rank == 0) {
        A = (double *)malloc(N * N * sizeof(double));
        C = (double *)malloc(N * N * sizeof(double));

        printf("Iniciando Multiplicacion Hibrida MPI + OpenMP\n");
        printf("Matriz: %d x %d | Procesos MPI: %d | Hilos OMP: %d\n", N, N, size, hilos_omp);

        for (int i = 0; i < N * N; i++) {
            A[i] = 1.0; B[i] = i % 10;
        }
        inicio_tiempo = MPI_Wtime();
    }

    MPI_Bcast(B, N * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(A, filas_por_proceso * N, MPI_DOUBLE, local_A, filas_por_proceso * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    #pragma omp parallel for shared(local_A, B, local_C)
    for (int i = 0; i < filas_por_proceso; i++) {
        for (int j = 0; j < N; j++) {
            double suma_temporal = 0.0;
            for (int k = 0; k < N; k++) {
                suma_temporal += local_A[i * N + k] * B[k * N + j];
            }
            local_C[i * N + j] = suma_temporal;
        }
    }

    MPI_Gather(local_C, filas_por_proceso * N, MPI_DOUBLE, C, filas_por_proceso * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        fin_tiempo = MPI_Wtime();
        printf("Calculo completado. Tiempo: %f segundos.\n\n", fin_tiempo - inicio_tiempo);
        print_matrix_preview(C, N, N, "C (Resultado Final)");
        free(A); free(C);
    }

    free(B); free(local_A); free(local_C);
    MPI_Finalize();
    return 0;
}

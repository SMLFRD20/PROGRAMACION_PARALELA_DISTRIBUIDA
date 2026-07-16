#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_ITER 10
#define CHK_INTERVAL 4
#define FAIL_ITER 6

int main(int argc, char** argv) {
    int rank, size;
    int iteracion_actual = 0;
    long suma_parcial = 0;
    int es_recuperacion = 0;
    char filename[256];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 3) {
        if (rank == 0) printf("Error: Se requieren al menos 3 procesos MPI.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    sprintf(filename, "checkpoint_proc_%d.dat", rank);

    FILE* file = fopen(filename, "rb");
    if (file != NULL) {
        fread(&iteracion_actual, sizeof(int), 1, file);
        fread(&suma_parcial, sizeof(long), 1, file);
        fclose(file);
        es_recuperacion = 1;
        printf("[Proceso %d] ESTADO RECUPERADO -> Retomando desde iteración %d (Suma: %ld)\n", 
               rank, iteracion_actual, suma_parcial);
    } else {
        printf("[Proceso %d] INICIO LIMPIO -> Configurando primer estado.\n", rank);
    }

    MPI_Barrier(MPI_COMM_WORLD); 
    if (rank == 0 && iteracion_actual == 0) printf("--- Iniciando cómputo ---\n");

    for (int i = iteracion_actual + 1; i <= MAX_ITER; i++) {
        iteracion_actual = i;
        suma_parcial += rank * i;
        
        sleep(1);

        if (i % CHK_INTERVAL == 0) {
            MPI_Barrier(MPI_COMM_WORLD); 
            
            if (rank == 0) printf("\n[COORD] Guardando Checkpoint Global en iteración %d...\n", i);
            
            file = fopen(filename, "wb");
            if (file != NULL) {
                fwrite(&iteracion_actual, sizeof(int), 1, file);
                fwrite(&suma_parcial, sizeof(long), 1, file);
                fclose(file);
            }
            
            MPI_Barrier(MPI_COMM_WORLD); 
        }

        printf("[Proceso %d] Completó iteración %d | Suma actual: %ld\n", rank, i, suma_parcial);

        if (i == FAIL_ITER && es_recuperacion == 0) {
            if (rank == 1) {
                printf("\n[Proceso %d] SIMULANDO FALLO CRÍTICO. Abortando ejecución...\n\n", rank);
                exit(1);
            }
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    if (rank == 0) printf("\n--- Procesamiento finalizado con éxito ---\n");
    
    MPI_Finalize();
    return 0;
}
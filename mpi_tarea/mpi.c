#include <stdio.h>
#include "C:\Program Files (x86)\Microsoft SDKs\MPI\Include\mpi.h"

int main(int argc, char *argv[]) {
    int rango_proceso;
    int total_procesos;
    int dato_a_compartir;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rango_proceso);
    MPI_Comm_size(MPI_COMM_WORLD, &total_procesos);

    if (rango_proceso == 0) {
        dato_a_compartir = 100;
        MPI_Send(&dato_a_compartir, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
    } 
    else if (rango_proceso == 1) {
        MPI_Recv(&dato_a_compartir, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Soy el proceso 1 y he recibido el valor %d con éxito.\n", dato_a_compartir);
    }

    MPI_Finalize();
    return 0;
}
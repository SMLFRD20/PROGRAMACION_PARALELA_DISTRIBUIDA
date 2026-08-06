from mpi4py import MPI
import random

def main():
    comm = MPI.COMM_WORLD
    rank = comm.Get_rank()
    size = comm.Get_size()

    N = None
    if rank == 0:
        N = int(input("Ingrese la cantidad de valores por proceso (N): "))

    N = comm.bcast(N, root=0)

    local_sum = 0.0
    random.seed(rank)
    for _ in range(N):
        local_sum += random.uniform(0.0, 100.0)

    global_sum = comm.reduce(local_sum, op=MPI.SUM, root=0)

    average = None
    if rank == 0:
        average = global_sum / (N * size)

    average = comm.bcast(average, root=0)

    print(f"Proceso {rank} - Promedio recibido: {average}")

if __name__ == "__main__":
    main()
import os
os.environ['NUMBA_CUDA_DRIVER'] = '/usr/lib/x86_64-linux-gnu/libcuda.so.1'

import numpy as np
from numba import cuda, njit, prange
import time

@cuda.jit
def sumar_vectores_gpu(A, B, C, n):
    i = cuda.grid(1)
    if i < n:
        C[i] = A[i] + B[i]

@njit(parallel=True)
def sumar_vectores_cpu(A, B, C, n):
    for i in prange(n):
        C[i] = A[i] + B[i]

def main():
    if not cuda.is_available():
        print("==========================================================")
        print("¡ADVERTENCIA! No se detectó ninguna GPU de NVIDIA.")
        print("Ve a 'Entorno de ejecución' > 'Cambiar tipo de entorno'")
        print("y selecciona 'T4 GPU'. Luego vuelve a correr esta celda.")
        print("==========================================================")
        return

    N = 1048576
    A = np.arange(N, dtype=np.float32)
    B = np.arange(N, dtype=np.float32) * 2.0
    C_cpu = np.zeros(N, dtype=np.float32)
    C_gpu = np.zeros(N, dtype=np.float32)

    sumar_vectores_cpu(A, B, C_cpu, N)

    inicio_cpu = time.time()
    sumar_vectores_cpu(A, B, C_cpu, N)
    tiempo_cpu = time.time() - inicio_cpu
    print(f"Tiempo CPU: {tiempo_cpu:.6f} segundos")

    d_A_dummy = cuda.to_device(np.zeros(1, dtype=np.float32))
    d_B_dummy = cuda.to_device(np.zeros(1, dtype=np.float32))
    d_C_dummy = cuda.device_array(1, dtype=np.float32)
    sumar_vectores_gpu[1, 1](d_A_dummy, d_B_dummy, d_C_dummy, 1)

    inicio_gpu = time.time()
    d_A = cuda.to_device(A)
    d_B = cuda.to_device(B)
    d_C = cuda.device_array(N, dtype=np.float32)

    hilos_por_bloque = 256
    bloques = (N + hilos_por_bloque - 1) // hilos_por_bloque

    sumar_vectores_gpu[bloques, hilos_por_bloque](d_A, d_B, d_C, N)
    cuda.synchronize()

    d_C.copy_to_host(C_gpu)
    tiempo_gpu = time.time() - inicio_gpu
    print(f"Tiempo GPU: {tiempo_gpu:.6f} segundos")

    if np.allclose(C_cpu, C_gpu):
        print("Verificacion exitosa.")
    else:
        print("Error en verificacion.")

    print(f"Speedup: {tiempo_cpu / tiempo_gpu:.2f}X")

if __name__ == '__main__':
    main()
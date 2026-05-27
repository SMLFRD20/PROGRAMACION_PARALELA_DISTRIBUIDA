#include <stdio.h>
#include <windows.h>

int contador_compartido = 0;
HANDLE semaforo;

DWORD WINAPI incrementar_contador(LPVOID arg) {
    (void)arg;
    for (int i = 0; i < 1000; i++) {
        DWORD wait_result = WaitForSingleObject(semaforo, INFINITE);
        if (wait_result == WAIT_OBJECT_0) {
            contador_compartido++;
            ReleaseSemaphore(semaforo, 1, NULL);
        } else {
            fprintf(stderr, "Error esperando el semáforo. Código: %lu\n", GetLastError());
            break;
        }
    }
    return 0;
}

int main() {
    HANDLE hilos[5] = {0};
    int hilos_creados = 0;
    
    semaforo = CreateSemaphore(NULL, 1, 1, NULL);
    if (semaforo == NULL) {
        fprintf(stderr, "Error al crear el semáforo. Código de error: %lu\n", GetLastError());
        return 1;
    }
    
    printf("Iniciando programa (Version Windows Nativa)...\n");
    printf("Se crearan 5 hilos. Cada uno sumara 1000 al contador.\n");

    for (int i = 0; i < 5; i++) {
        hilos[i] = CreateThread(
            NULL,
            0,
            incrementar_contador,
            NULL,
            0,
            NULL
        );
        if (hilos[i] == NULL) {
            fprintf(stderr, "Error al crear el hilo %d. Código de error: %lu\n", i, GetLastError());
            for (int j = 0; j < hilos_creados; j++) {
                CloseHandle(hilos[j]);
            }
            CloseHandle(semaforo);
            return 1;
        }
        hilos_creados++;
    }

    DWORD wait_result = WaitForMultipleObjects(5, hilos, TRUE, INFINITE);
    if (wait_result == WAIT_FAILED) {
        fprintf(stderr, "Error en WaitForMultipleObjects. Código de error: %lu\n", GetLastError());
    }

    for (int i = 0; i < 5; i++) {
        if (hilos[i] != NULL) {
            CloseHandle(hilos[i]);
        }
    }
    CloseHandle(semaforo);
    
    printf("\nTrabajo finalizado con exito.\n");
    printf("Valor esperado: 5000\n");
    printf("Valor final de la variable compartida: %d\n", contador_compartido);
    
    return 0;
}
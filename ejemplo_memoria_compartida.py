import threading

contador_global = 0
candado = threading.Lock()

def trabajador(id_trabajador):
    global contador_global
    print(f"Trabajador {id_trabajador} empezando su tarea...")
    for _ in range(100000):
        with candado:
            contador_global += 1
    print(f"Trabajador {id_trabajador} terminó su turno.")

if __name__ == "__main__":
    hilos = []
    print("Iniciando el programa con memoria compartida...\n")
    for i in range(4):
        hilo = threading.Thread(target=trabajador, args=(i+1,))
        hilos.append(hilo)
        hilo.start()
    for hilo in hilos:
        hilo.join()
    print(f"\nTrabajo finalizado.")
    print(f"El valor final en la pizarra es: {contador_global}")
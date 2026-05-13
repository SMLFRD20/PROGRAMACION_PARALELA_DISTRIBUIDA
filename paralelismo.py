import multiprocessing
import time

def calcular_cubo(numero):
    resultado = numero * numero * numero
    print(f"El cubo de {numero} es {resultado}")

if __name__ == "__main__":
    numeros = [2, 4, 6, 8, 10]
    
    print("Iniciando procesamiento paralelo...")
    tiempo_inicio = time.time()

    pool = multiprocessing.Pool(processes=multiprocessing.cpu_count())
  
    pool.map(calcular_cubo, numeros)

    pool.close()
    pool.join()
    
    tiempo_fin = time.time()
    print(f"Procesamiento terminado en {tiempo_fin - tiempo_inicio:.4f} segundos.")
import threading
import time
import random

# NODOS

nodos = [1, 2, 3]
estado = {1: 'FOLLOWER', 2: 'FOLLOWER', 3: 'FOLLOWER'}
vivo = {1: True, 2: True, 3: True}
lider_actual = None
simulacion_activa = True

# ELECCION DE LIDER

def ciclo_nodo(nodo_id):
    global lider_actual
    while simulacion_activa:
        if not vivo[nodo_id]:
            break
            
        if estado[nodo_id] == 'FOLLOWER':
            tiempo_espera = random.uniform(1.5, 3.5)
            tiempo_inicio = time.time()
            
            while time.time() - tiempo_inicio < tiempo_espera:
                if lider_actual is not None and vivo[lider_actual]:
                    tiempo_inicio = time.time()
                time.sleep(0.1)
                
            if lider_actual is None or not vivo[lider_actual]:
                if estado[nodo_id] == 'FOLLOWER' and vivo[nodo_id]:
                    iniciar_eleccion(nodo_id)
        
        elif estado[nodo_id] == 'LEADER':
            time.sleep(1.0)
            if vivo[nodo_id]:
                print(f"Nodo {nodo_id} enviando latido a los seguidores.")
                
        else:
            time.sleep(0.1)

def iniciar_eleccion(nodo_id):
    global lider_actual
    estado[nodo_id] = 'CANDIDATE'
    print(f"Temporizador del nodo {nodo_id} agotado, inicia elección.")
    
    votos = 1
    print(f"El nodo {nodo_id} vota por sí mismo.")
    
    for n in nodos:
        if n != nodo_id and vivo[n] and estado[n] != 'CANDIDATE':
            print(f"El nodo {n} otorga su voto al candidato {nodo_id}.")
            votos += 1
            
    if votos >= 2:
        estado[nodo_id] = 'LEADER'
        lider_actual = nodo_id
        print(f"El nodo {nodo_id} es el nuevo líder (votos: {votos}).")
        for n in nodos:
            if n != nodo_id and vivo[n]:
                estado[n] = 'FOLLOWER'

# REPLICACION

def replicar_dato(dato):
    if lider_actual is None or not vivo[lider_actual]:
        print("Error: no hay líder para replicar.")
        return
        
    print(f"\nEl líder {lider_actual} intenta replicar el valor {dato}.")
    confirmaciones = 1
    
    for n in nodos:
        if n != lider_actual and vivo[n]:
            print(f"El nodo {n} confirma el valor {dato}.")
            confirmaciones += 1
            
    if confirmaciones >= 2:
        print(f"Consenso alcanzado, el líder confirma el valor {dato}.\n")

# SIMULACION

if __name__ == "__main__":
    print("Iniciando la simulación del algoritmo Raft.")
    
    hilos = []
    for n in nodos:
        h = threading.Thread(target=ciclo_nodo, args=(n,))
        h.start()
        hilos.append(h)
        
    time.sleep(4)
    
    replicar_dato("A=1")
    
    time.sleep(2)
    
    if lider_actual is not None:
        print(f"\nFallo detectado: el nodo {lider_actual} (líder) ha dejado de responder.\n")
        vivo[lider_actual] = False
        
    time.sleep(6)
    
    simulacion_activa = False
    for h in hilos:
        h.join()
        
    print("Fin de la simulación.")

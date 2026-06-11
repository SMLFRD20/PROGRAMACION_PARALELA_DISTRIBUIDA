import xmlrpc.client

def iniciar_cliente():
    print("Conectando al servidor RPC...")
    servidor = xmlrpc.client.ServerProxy('http://localhost:8000')
    
    try:
        entrada = input("Por favor, ingresa un numero entero para calcular su cuadrado: ")
        numero = int(entrada)
        
        print("Enviando peticion al servidor remoto...")
        resultado = servidor.calcular_cuadrado(numero)
        
        print("El servidor calculo que el cuadrado de", numero, "es", resultado)
        
    except ValueError:
        print("Error: Debes ingresar un numero entero valido.")
    except ConnectionRefusedError:
        print("Error: No se pudo contactar al servidor. Asegurate de que este encendido.")

if __name__ == '__main__':
    iniciar_cliente()

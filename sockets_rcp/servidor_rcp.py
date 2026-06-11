from xmlrpc.server import SimpleXMLRPCServer

def calcular_cuadrado(numero):
    print("El cliente solicito el cuadrado del numero", numero)
    resultado = numero * numero
    return resultado

def iniciar_servidor():
    servidor = SimpleXMLRPCServer(('localhost', 8000))
    print("Servidor RPC encendido y esperando peticiones en el puerto 8000...")
    
    servidor.register_function(calcular_cuadrado, "calcular_cuadrado")
    
    servidor.serve_forever()

if __name__ == '__main__':
    iniciar_servidor()

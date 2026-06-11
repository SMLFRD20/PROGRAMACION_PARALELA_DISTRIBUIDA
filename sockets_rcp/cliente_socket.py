import socket

def iniciar_cliente():
    cliente = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    
    try:
        cliente.connect(('localhost', 5000))
        print("Conexion establecida con el servidor.")
        
        mensaje = "Hola servidor. Esta es mi prueba de Socket."
        cliente.send(mensaje.encode('utf-8'))
        
        respuesta = cliente.recv(1024).decode('utf-8')
        print("Respuesta del servidor:", respuesta)
        
    except ConnectionRefusedError:
        print("Error: El servidor rechazo la conexion. Asegurate de que este encendido.")
    finally:
        cliente.close()

if __name__ == '__main__':
    iniciar_cliente()

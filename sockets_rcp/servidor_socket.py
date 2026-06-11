import socket

def iniciar_servidor():
    servidor = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    servidor.bind(('localhost', 5000))
    servidor.listen(1)
    
    print("Servidor Socket esperando conexiones en el puerto 5000...")
    
    conexion, direccion = servidor.accept()
    print("Conexion aceptada desde la direccion", direccion)
    
    mensaje_recibido = conexion.recv(1024).decode('utf-8')
    print("El cliente nos envio:", mensaje_recibido)
    
    respuesta = "Mensaje recibido correctamente. Confirmacion enviada."
    conexion.send(respuesta.encode('utf-8'))
    
    conexion.close()
    servidor.close()
    print("Conexion finalizada exitosamente.")

if __name__ == '__main__':
    iniciar_servidor()

import socket
import time

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

sock.bind(("127.0.0.1", 13))

sock.listen(5)

print("Servidor Daytime TCP escuchando en 127.0.0.1:13...")

while True:
    conexion, cliente = sock.accept()

    print(f"Conexion recibida de {cliente}")

    respuesta = time.ctime() + "\r\n"

    conexion.sendall(respuesta.encode("ascii"))

    conexion.close()
import socket
import time

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind(("127.0.0.1", 13))

print("Servidor Daytime UDP escuchando en 127.0.0.1:13...")

while True:
    datos, cliente = sock.recvfrom(1024)

    respuesta = time.ctime() + "\r\n"

    sock.sendto(respuesta.encode("ascii"), cliente)

    print(f"Consulta recibida de {cliente}")
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#define TAM_BUFFER 1024

int main(int argc, char *argv[]) {
    int sock;
    char buffer[TAM_BUFFER];

    struct sockaddr_in servidor;

    if (argc != 2){
        printf("Uso: %s <direccion IP del servidor>\n", argv[0]);
        return 1;
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock == -1) {
        perror("Error al crear el socket");
        return 1;
    }

    

    memset(&servidor, 0, sizeof(servidor));

    servidor.sin_family = AF_INET;
    servidor.sin_port = htons(13);

    if (inet_aton(argv[1], &servidor.sin_addr) == 0){
        printf("Direccion IP no valida\n");
        close(sock);
        return 1;
    }
        if (connect(sock,
                (struct sockaddr *)&servidor,
                sizeof(servidor)) == -1){
        perror("No se pudo conectar al servicio Daytime");
        close(sock);
        return 1;
    }

    printf("Conexion TCP establecida con el servidor.\n");
    int bytes;

    bytes = read(sock, buffer, TAM_BUFFER - 1);
    if (bytes == -1) {
        perror("Error al recibir datos");
        close(sock);
        return 1;
    }

    buffer[bytes] = '\0';

    printf("Hora del servidor: %s\n", buffer);
    close(sock);
    return 0;
}
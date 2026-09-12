#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define TAM_BUFFER 1024

int main(int argc, char *argv[]){
    int sock;
    int resultado;
    int segundos = 1;

    char buffer[TAM_BUFFER];
    struct sockaddr_in servidor;
    struct timeval timeout;
    fd_set conjunto;

    if (argc < 2 || argc > 3){
        printf("Uso: %s <direccion IP del servidor> [timeout]\n", argv[0]);
        return 1;
    }

    if (argc == 3) {
        segundos = atoi(argv[2]);
    }

    sock = socket(AF_INET, SOCK_DGRAM, 0);

    if (sock == -1)
    {
        perror("Error al crear el socket");
        return 1;
    }

    memset(&servidor, 0, sizeof(servidor));

    servidor.sin_family = AF_INET;
    servidor.sin_port = htons(13);

    if (inet_aton(argv[1], &servidor.sin_addr) == 0)
    {
        printf("Direccion IP no valida\n");
        close(sock);
        return 1;
    }

    if (sendto(sock,
            NULL,
            0,
            0,
            (struct sockaddr *)&servidor,
            sizeof(servidor)) == -1)
    {
        perror("Error al enviar el datagrama");
        close(sock);
        return 1;
    }

    printf("Datagrama UDP vacio enviado al servidor.\n");
    FD_ZERO(&conjunto);
    FD_SET(sock, &conjunto);

    timeout.tv_sec = segundos;
    timeout.tv_usec = 0;

    resultado = select(sock + 1,
                   &conjunto,
                   NULL,
                   NULL,
                   &timeout);
    
    if (resultado == -1){
        perror("Error en select");
    }
    else if (resultado == 0){
        printf("Timeout vencido. No se recibio respuesta.\n");
    }
    else{
         int bytes;
        bytes = recv(sock, buffer, TAM_BUFFER - 1, 0);
        if (bytes == -1){
            perror("Error al recibir");
        }
        else{
            buffer[bytes] = '\0';
            printf("Hora del servidor: %s\n", buffer);
        }
    }

    close(sock);
    return 0;

    
}
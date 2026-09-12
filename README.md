# Clientes Daytime TCP/UDP

Práctica de **Redes II** orientada al desarrollo y análisis de clientes del servicio **Daytime** utilizando los protocolos de transporte **UDP y TCP**.

El servicio Daytime utiliza el **puerto 13** y devuelve una cadena de texto con la fecha y hora del servidor. La práctica permite observar las diferencias entre un protocolo no orientado a conexión como UDP y uno orientado a conexión como TCP. 

## Objetivos

- Implementar un cliente Daytime utilizando UDP.
- Implementar un cliente Daytime utilizando TCP.
- Trabajar con sockets IPv4 en C.
- Analizar el tráfico generado mediante Wireshark.
- Observar puertos, datagramas, segmentos, números de secuencia, ACK, flags y ventanas TCP.

## Estructura

```text
Clientes-daytime-TCP-UDP/
├── horaUDP.c
├── horaTCP.c
├── servidor_daytime.py
└── README.md

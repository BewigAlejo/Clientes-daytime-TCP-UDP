# Práctica 3: clientes Daytime con UDP y TCP

Trabajo de **Redes II** sobre el servicio Daytime. Se implementaron dos clientes IPv4 en C que consultan la fecha y hora de un servidor en el **puerto 13**: uno usa UDP y el otro TCP. También se agregaron dos servidores locales en Python para probarlos sin depender de que un equipo externo tenga habilitado Daytime.

## Archivos

| Archivo | Función |
| --- | --- |
| `horaUDP.c` | Cliente UDP: envía un datagrama sin datos y espera una respuesta con timeout. |
| `horaTCP.c` | Cliente TCP: se conecta, lee la respuesta y cierra el socket. |
| `servidor_daytime_udp.py` | Servidor local de prueba UDP en `127.0.0.1:13`. |
| `servidor_daytime_tcp.py` | Servidor local de prueba TCP en `127.0.0.1:13`. |
| `horaUDP.exe`, `horaTCP.exe` | Ejecutables del entorno de la práctica; conviene recompilarlos. |

## Funcionamiento

El cliente UDP se invoca como `horaUDP <IP> [timeout]`. La dirección debe ser una IPv4 numérica. El tiempo de espera opcional se expresa en segundos y vale **1 segundo** si se omite. El cliente crea un socket `AF_INET`/`SOCK_DGRAM`, configura `sockaddr_in` con `htons(13)` e `inet_aton()`, y envía con `sendto()` un datagrama de **0 bytes de aplicación**. Luego prepara un `fd_set` y un `timeval` para que `select()` espere una respuesta sin quedar bloqueado indefinidamente. Si hay datos, `recv()` los copia en un buffer, se agrega el terminador `\0` y se imprime la hora. Si no llegan datos en el plazo previsto, se informa el timeout. Por último se cierra el socket.

El cliente TCP se invoca como `horaTCP <IP>`. Crea un socket `AF_INET`/`SOCK_STREAM`, configura el mismo destino y llama a `connect()`. El servicio Daytime envía la fecha y hora al aceptar la conexión: **el cliente no envía datos de aplicación**. El programa lee una vez con `read()`, termina la cadena, la muestra y cierra el socket. Informa errores de IP, conexión y lectura. Este cliente no implementa un timeout propio; el tiempo de espera de `connect()` depende del sistema operativo.

Ambos clientes dejan que el sistema asigne un puerto de origen efímero. Leen como máximo 1023 bytes para reservar una posición del buffer de 1024 para `\0`. Los servidores Python usan `time.ctime()` para obtener la fecha y hora del **equipo donde se ejecutan**, agregan `\r\n` y la envían como ASCII. El servidor UDP responde a la IP y el puerto devueltos por `recvfrom()`. El servidor TCP acepta cada conexión, responde con `sendall()` y cierra esa conexión, pero continúa escuchando. Ambos están ligados solo a `127.0.0.1`, por lo que sirven para pruebas en la misma computadora.

**Limitación:** el argumento `timeout` de UDP se convierte mediante `atoi()` sin comprobar que sea un entero positivo. Para las pruebas se deben usar valores válidos, como `1` o `5`.

## Cómo probarlo con el repositorio descargado

Se necesitan **Python 3** y **GCC compatible con las cabeceras POSIX** usadas por los clientes (`sys/socket.h`, `unistd.h`, etc.). En la práctica se usó Windows con **MSYS2, terminal MSYS**, para C y PowerShell para Python. También se puede usar Linux con GCC y Python 3. En Windows, un GCC de MinGW para programas nativos no equivale a este entorno POSIX.

1. Abrir una terminal en la carpeta descargada y compilar:

   ```sh
   gcc horaUDP.c -o horaUDP
   gcc horaTCP.c -o horaTCP
   ```

   En MSYS2 se generan `horaUDP.exe` y `horaTCP.exe`, que se ejecutan como `./horaUDP` y `./horaTCP`. Si `gcc` no se reconoce, abrir una terminal MSYS de MSYS2 que tenga GCC instalado.

2. Para probar **UDP**, iniciar el servidor en otra terminal situada en la carpeta del proyecto y dejarlo abierto:

   ```sh
   python servidor_daytime_udp.py
   ```

   En la terminal del cliente:

   ```sh
   ./horaUDP 127.0.0.1
   ./horaUDP 127.0.0.1 5
   ```

   Se debe ver `Datagrama UDP vacio enviado al servidor.` seguido de `Hora del servidor: ...`. El servidor registra la consulta. Si no llega respuesta antes del plazo, se muestra `Timeout vencido. No se recibio respuesta.`.

3. Para probar **TCP**, iniciar el segundo servidor en otra terminal (puede detenerse el UDP con `Ctrl+C`; TCP y UDP usan puertos independientes):

   ```sh
   python servidor_daytime_tcp.py
   ```

   En la terminal del cliente:

   ```sh
   ./horaTCP 127.0.0.1
   ```

   Deben aparecer `Conexion TCP establecida con el servidor.` y `Hora del servidor: ...`. La hora concreta depende del reloj de la computadora.

4. Para probar un servicio no disponible, detener el servidor TCP con `Ctrl+C` y repetir `./horaTCP 127.0.0.1`. Si no hay otro proceso escuchando en TCP/13, `connect()` informará un error como `Connection refused`. Con el servidor UDP detenido, la consulta UDP normalmente vence por timeout; según el sistema también puede llegar un error de red.

Si Python no puede asociarse al puerto 13, comprobar que esté libre y que el entorno permita utilizarlo. En sistemas Unix, los puertos menores que 1024 suelen requerir privilegios. Los clientes aceptan **IPv4 numéricas**, no nombres DNS. Se puede reemplazar `127.0.0.1` por la IP de un servidor remoto que realmente ofrezca Daytime en el protocolo elegido y sea accesible por el puerto 13.

## Capturas y análisis de Wireshark

Para repetir la captura local, seleccionar la interfaz de **loopback**, iniciar la captura antes de ejecutar el cliente y aplicar el filtro de visualización `udp.port == 13` o `tcp.port == 13`. Para destinos externos se debe capturar en la interfaz de salida; por ejemplo, el filtro `tcp.port == 13 && ip.addr == 147.156.3.0` acota una de las pruebas. Los puertos efímeros, tiempos, secuencias y ventanas siguientes son **mediciones de la práctica**, no valores fijos del código.

### UDP

Primero se enviaron datagramas vacíos a `11.0.0.2`. Wireshark confirmó `Len=0`, pero no llegó una respuesta antes del timeout, tanto con el valor predeterminado como con 5 segundos. También se probó una IP obtenida al resolver `echo.u-blox.com`, sin recibir una respuesta Daytime. Esto verificó el envío, pero no permite atribuir la falta de respuesta a una causa específica.

Con el servidor local y `./horaUDP 127.0.0.1` se recibió la hora. La captura mostró **2 datagramas**: petición de `127.0.0.1:51006` a `127.0.0.1:13` con 0 bytes de aplicación, y respuesta en sentido inverso con 26 bytes (24 caracteres de fecha y hora más `\r\n`). Al sumar la cabecera UDP de 8 bytes, sus longitudes UDP fueron **8 y 34 bytes**. Entre la petición y la respuesta se midieron aproximadamente **1,03 ms** sobre loopback. UDP no tiene segmentos de establecimiento ni cierre de conexión.

### TCP

Durante el desarrollo se comprobó que cerrar el socket después de `connect()` sin llamar a `read()` impedía mostrar la respuesta. Se incorporó la lectura antes del cierre y el cliente obtuvo correctamente la hora del servidor local.

En la sesión exitosa analizada se observaron **9 segmentos**: 3 de establecimiento (`SYN`, `SYN-ACK`, `ACK`), 2 para la respuesta Daytime y su confirmación, y 4 para el cierre con `FIN` y `ACK`. El cliente usó el puerto efímero `53116`; envió **0 bytes de aplicación** y recibió **26 bytes**. La captura registró aproximadamente **0,3792 ms** para el proceso completo, **0,0832 ms** para establecer la conexión y **0,0391 ms** para cerrarla.

Se observaron los flags `SYN`, `ACK` y `FIN`. Los `SYN` anunciaron ventana de 65535 bytes y las opciones MSS (65495 bytes), Window Scale (factor 256), SACK Permitted y NOP. Con ese factor de escala, los segmentos posteriores inspeccionados anunciaron una ventana efectiva de 65280 bytes. La ventana del cliente no cambió entre el ACK previo y el posterior a recibir los 26 bytes. TCP numera bytes: `SYN` y `FIN` consumen cada uno una posición de secuencia; los 26 bytes de la respuesta avanzan otras 26 posiciones. En la captura, los valores absolutos de `Seq` observados abarcaron 1578005105–1578005106 para el cliente y 814808912–814808940 para el servidor.

También se compararon dos fallas de conexión. Con el servidor local detenido, `127.0.0.1:13` rechazó la conexión: se capturaron **5 SYN**, cada uno seguido de `RST-ACK`, separados aproximadamente **0,5 s** y con el mismo puerto cliente (`59554`); el programa terminó con `Connection refused`. Hacia `147.156.3.0:13` se capturaron **5 SYN sin respuesta TCP visible** (el inicial y cuatro retransmisiones), separados aproximadamente **1, 2, 4 y 8 s**, con el mismo puerto cliente (`63546`); se obtuvo `Connection timed out`. El código realiza una sola llamada a `connect()` en ambos casos: la cantidad y el ritmo de los reintentos dependen del sistema y la red.

## Material de referencia

La consigna de la práctica describe Daytime sobre UDP y TCP y las preguntas para analizar las capturas. El apunte de funciones de sockets sirvió de referencia para las llamadas de red usadas en C. El informe de la práctica contiene las capturas y mediciones resumidas aquí. Los PDF están fuera de este repositorio; el código y las instrucciones necesarias para ejecutarlo están en esta carpeta.

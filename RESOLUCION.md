### RESOLUCION TP4 - SOCKETS

## ESPECIFICACION ADMINISTRACION DE SERVIDOR DE PORTERO

La aplicacion de desarrollo con el modelo Cliente/Servidor, en el que cada cliente conectado puede enviar comandos que el servidor procesara y enviara una respuesta como resultado de la operacion realizada.

## Servidor

La implementacion del servidor se realizo como un proceso principal que administra las coneccciones y por cada cliente que se conecta y realiza una peticion, genera un thread concurrente, que gestiona y procesa la solicitud, la operacion termina con una respuesta al cliente y la finalizacion del thread.
El protocolo de comunicacion utilizado entre el cliente y el thread del servidor depende de la operacion:
* TPC para las operaciones de apagar, encender y programar luces, activar/desactivar y programar riego.
* FTP envio imagen portero.
* UDP contestar y recibir llamanda portero.
En el caso de la programacion del encendido de las luces y el riego se realiza a traves de un archivo de configuracion, que almacena los parametros, y al que accede cualquier thread que lea/escriba. Para ello se protegio el archivo mediando mutex.


## Cliente

La implementacion del cliente se realiza mediante un menu de usuario en el que se prensentan las distintas los comandos que el usuario puede ejecutar:
* Luces: prender, apagar, programar (hora minutos duracion).
* Riego: prender, apagar, programar (hora minutos duracion).
* Envio imagen: solicitar.
* Llamanda portero: recibir contestar.

En la comunicacion con el servidor de cada comando se realizan las siguientes:
* prender luces: se envia opcion 1 ON, se recibe respuesta resultado de la operacion desde el servidor.
* apagar luces: se envia opcion 1 OFF, se recibe respuesta resultado de la operacion desde el servidor.
* programar luces: se envia opcion 1 hora minuto duracion, se recibe resultado de la operacion desde el servidor.
* prender riego: se envia opcion 2 ON, se recibe resultado de la operacion desde servidor.
* apagar riego: se envia opcion 2 OFF, se recibe resultado de la operacion desde servidor.
* envio de imagen: se envia opcion 3, se procesa la transferecia de bloques del archivo con el servidor, al finalizar, se recibe resultado de la operacion desde servidor.
* llamada del portero: se envia opcion 4, se abre el archivo de la conversacion, se envia contestacion y el servidor responde iterativamente hasta terminar. Finalizada la conversacion de cierran los archivos.
En todos los casos luego de finalizacion de un comando el cliente vuelve al menu principal, y el servidor culmina el thread y vuelve al proceso principal.




  



### RESOLUCION TP4 - SOCKETS

## 

* Servidor: concurrente con Threads

* Si es concurrente conviene utilizar pthreads

* Comunicaciones TPC: comandos apagar y encender luces, activar/desactivar riego, aviso llamada portero, envio imagen portero

* Cliente:
comandos:
   - encenderLuces()
   - apagarLuces()
  - programarEncendidoLuces( hora, minutos, duracion en minutos )
  
  - activarRiego()
  - desactivarRiego()
  - programarRiego( hora, minutos, duracion )
  - solicitarImagenPortero()
  - contestarLlamada( archivoAudio )
  - recivirAvisoPortero()
  

* Servidor:
comandos:
    - encenderLuces()
    - apagarLuces()
    - programarLuces( horaEncendido, minutosEncendido, duracion )
    
    - activarRiego()
    - desactivarRiego()
    - programarRiego( HoraEncendido, minutoEncendido, duracion )

    - enviarAvisoPortero()
    - enviarImagenPortero()
    - recivirLlamada()
    - enviarLlamada()

* Comunicaciones UDP: envio/recepcion de audio. (administrar conexion)

* Proceso generador de llamadas al portero en el servidor.

* archivo de configuracion: leer , y bloquear para escribir (utilizar mutex)



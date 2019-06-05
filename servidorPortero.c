#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

#include "servidorPortero.h"
#include "porteroUtils.h"

static pthread_mutex_t filelock = PTHREAD_MUTEX_INITIALIZER;
static int ESTADO_LUCES = 0;
static int ESTADO_RIEGO = 0;

void *atenderPeticionTCP( void *d ) {
	int total;	
	char msg[ MAXLINEA ];
	int sockTCP = (int)d;
	//msg = (char*)malloc(MAXLINEA);
	
   	printf( "(%d) Atendiendo petición.\n", sockTCP );
	while ( total = recibir( sockTCP, msg ) > 0 ) {
		//printf( "(%d) Recibido: %s\n", sockTCP, msg );
		
		/*-------------------------------------------------------* 
		* Realizar la tarea específica del servicio
		*-------------------------------------------------------*/
		procesarTCP( msg, sockTCP );
	
		/*-------------------------------------------------------* 
	 	* Responder petición		      					
		*-------------------------------------------------------*/
		if ( ( total = enviar( sockTCP, msg ) ) < 0 ) { 
			perror("(%d) ERROR ENVIAR TCP: ");
			exit(-1);
		}
		//printf("(%d) Respuesta enviada: %s.\n", sockTCP, msg );
	}
}

void *atenderPeticionUDP( void *d ) {
	struct sockaddr_in dir_cli, dir_cli_p;
	int recibido;
	socklen_t longitud;
	char msg[ MAXLINEA ];
	int sockUDP = (int)d;

	for(;;) {
		longitud = sizeof( dir_cli );
        bzero( msg,MAXLINEA );
	 	// Recibo el comando del cliente
		recibido = recvfrom( sockUDP, msg, MAXLINEA, 0, (struct sockaddr *) &dir_cli, &longitud );
	 	
		/*-------------------------------------------------------* 
		* Procesa el comando recibido y envia respuesta al cliente
		*-------------------------------------------------------*/
		 procesarUDP( msg, sockUDP, recibido );
	 
	}
	
}

int main ( int argc, char *argv[] ) {
	/*---------------------------------------------------------------------*
	 * Verificar los argumentos
	 *---------------------------------------------------------------------*/
 	if ( argc < 2 ) {
		printf( "Uso: servidor <puerto>\n" );
		exit( -1 );
	}
	printf( "\tServidor Portero. Para salir presione <Ctrl>-C\n" );
	
	/*--------------------------------------------------------------------* 
	 * Inicializar el servidor
	 *--------------------------------------------------------------------*/ 
	if ( ( inicializar( atoi( argv[1] ) ) ) < 0 ) {
		perror( "ERROR INICIALIZAR: " );
		exit(-1);
	}	

    //msg = (char*)malloc( sizeof( MAXLINEA + 1 ) );

}

/*-------------------------------------------------------------------------*
 * inicializar() - inicializar el servidor
 *-------------------------------------------------------------------------*/ 	    
int inicializar( int puerto ) {
	int sock_TCP, sock_UDP;
	int ndescriptor;
	int ldescriptorUDP;
	int opt = 1;  			//Opcion para reusar el puerto: setsockopt
	
	pthread_t t_hijoTCP;	//Estructura para atenderPeticionTCP	
	pthread_t t_hijoUDP;	//Estructura para atenderPeticionUDP
	struct sockaddr_in dir_srv, dir_cli;

	int recibido;
	socklen_t longitud;
	char msg[ MAXLINEA ];
	
	/*--------------------------------------------------------------------* 
	 * Inicializar el servidor TCP                          			  *
	 *--------------------------------------------------------------------*/
	if ( ( sock_TCP = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 ) {
		printf("ERROR SOCKET TCP:\n");
		exit ( -1 );
	}
	/*--------------------------------------------------------------------* 
	 * Inicializar el servidor UDP                          			  *
	 *--------------------------------------------------------------------*/
	if ( ( sock_UDP = socket( AF_INET, SOCK_DGRAM, 0 ) ) < 0 ) {
		perror("ERROR SOCKET UDP:\n");
		exit( -1 );
	}
	/* bind de la dirección local */
	bzero( (char *) &dir_srv, sizeof( dir_srv ) );
	dir_srv.sin_family = AF_INET;	/* utilizará familia de protocolos de Internet */
	dir_srv.sin_addr.s_addr = htonl( INADDR_ANY );		/* sobre la dirección IP local */
	dir_srv.sin_port = htons( puerto );

	setsockopt( sock_TCP, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof( opt ));
	setsockopt( sock_UDP, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof( opt ));

	// bind TCP
    int b;
	if ((b=bind( sock_TCP, (struct sockaddr *) &dir_srv, sizeof( dir_srv ) )) < 0 ){
		printf("ERROR BIND TCP:%d\n", b);
		exit ( -2 );
	}
	// bind UDP
	if ( bind( sock_UDP, (struct sockaddr *) &dir_srv, sizeof( dir_srv ) ) < 0 ) {
		printf("ERROR BIND UDP:\n");
		exit ( -2 );
	}

	/* armar una lista de espera para MAXCLIENTES clientes */	
	listen( sock_TCP, MAXCLIENTES );
	
	printf ( "\n\tEscuchando en puerto: %d, dirección %d. \n\tPodría tener hasta %d clientes esperando.\n", ntohs( dir_srv.sin_port ), ntohl( dir_srv.sin_addr.s_addr ), MAXCLIENTES );
	printf( "Servidor inicializado.\n" );
	
	/*----------------------------------------------------------------* 
	 * Comenzar tarea del servidor UDP
	 *----------------------------------------------------------------*/
	pthread_create( &t_hijoUDP, NULL, atenderPeticionUDP, (void *)sock_UDP );
	/*----------------------------------------------------------------*
	 * Comenzar tarea del servidor TCP
	 *----------------------------------------------------------------*/
	for(;;) { 
		/* 
	 	 * Esperar una petición
	 	 */
		printf( "Esperando petición (Padre).\n" );
		if ( ( ndescriptor = esperar(sock_TCP) ) < 0 ) {
			printf("ERROR ESPERANDO: ");
			exit( -3 );
		}
		
		pthread_create( &t_hijoTCP, NULL, atenderPeticionTCP, (void *)ndescriptor );
	}
    pthread_mutex_destroy( &filelock );
	return ( 1 );
}

/*-------------------------------------------------------------------------*
 * esperar()
 *-------------------------------------------------------------------------*/ 	    
int esperar( int sockfd ) {
	int nsockfd;

	/* aceptar una conexión */
	if ( ( nsockfd = accept( sockfd, 0, 0 ) ) < 0 )
		return ( -1 );
	
	return nsockfd;
}
		
/*-------------------------------------------------------------------------*
 * enviar() - enviar la respuesta al cliente 
 * retornar la longitud de la cadena enviada
 *-------------------------------------------------------------------------*/ 	    
int enviar( int nsockfd, char *msg ) {
	int longitud;
	
	if ( ( longitud = write( nsockfd, msg, strlen( msg ) ) ) < 0 ) {
		close( nsockfd );
		return ( -1 );
	}

	return longitud;
}

/*------------------------------------------------------------------------*
 * recibir() - recibir una petición
 * retornar la longitud de la cadena leída
 *------------------------------------------------------------------------*/ 	    
int recibir( int nsockfd, char *msg ) {
	int longitud;

	bzero( msg, MAXLINEA );

	if ( ( longitud =  read( nsockfd, msg, (int)MAXLINEA ) ) < 0 ) {
		close( nsockfd );
		return ( -2 );
	}
 	//if (longitud > 0) 
	//	printf( "comando = %s\n", msg );
	return ( longitud );
}

/*-----------------------------------------------------------------------* 
 * procesarTCP() - atender una petición
 *-----------------------------------------------------------------------*/
void procesarTCP( char *mensaje, int socketTCP ) {
	char **comando;
	int cp;
	int hora, minutos, duracion;
	char resp[MAXLINEA];

	printf ( "(%d) Procesando comando: %s \n", socketTCP, mensaje );
	cp = separarPalabras( mensaje, &comando );
	
	if (cp == 5){						//Si es PROG
		hora = atoi(comando[2]);
		minutos = atoi(comando[3]);
		duracion = atoi(comando[4]);
	}else{								//Si es ON | OFF	
		hora = -1;
		minutos = -1;
		duracion = -1;
	}
	
	switch (atoi(comando[0]))
	{
		case LUCES:
			atenderLuces(comando[1], hora, minutos, duracion, mensaje );
			break;
		case RIEGO:
			atenderRiego(comando[1], hora, minutos, duracion, mensaje );
			break;
		case IMAGEN:
			atenderImagen();
			break;
		case LLAMADA:
			//No atiendo este comando, lo atiende procesarUDP
			
			break;
		case EXIT:
			break;
		default:
			break;
	}	
}

/*-----------------------------------------------------------------------* 
 * procesarUDP() - Solo ejecuta la funcion de transferencia de audio
 *-----------------------------------------------------------------------*/
void procesarUDP( char *mensaje, int socketUDP, int recibido ) {
	
	atenderLlamada(mensaje, socketUDP, recibido);
}

/*-----------------------------------------------------------------------* 
 * atenderLuces() - ejecuta el comando LUCES ON | OFF | PROG
 *-----------------------------------------------------------------------*/
void atenderLuces(char *hacer, int hora, int minutos, int duracion, char *respuesta ){
	tConfig config;

	if (strcmp( hacer, "PROG" ) == 0) {			// PROG
		pthread_mutex_lock(&filelock);			//-----------------
		cargarConfig( &config );				//-----------------
		//config.luces = "LUCES";				//-----------------
		config.hluces = hora;					// Zona Critica
		config.mluces = minutos;				//-----------------
		config.dluces = duracion;				//-----------------
		guardarConfig( &config );	
        respuesta = "Luces Programadas";		//-----------------
		pthread_mutex_unlock(&filelock);		//-----------------
	} else if (strcmp(hacer, "ON") == 0) {		// ON
		if (ESTADO_LUCES == 0) {
            ESTADO_LUCES = 1;
            respuesta = "Luces prendidas";
        }
        else {
            respuesta = "Luces ya estan prendidas";
        }
	} else if (strcmp(hacer, "OFF") == 0) {		// OFF
		if (ESTADO_LUCES == 1) {
            ESTADO_LUCES = 0;
            respuesta = "Luces apagadas";
        }
        else {
            respuesta = "Luces ya estan apagadas";
        }
    }

}

/*-----------------------------------------------------------------------* 
 * atenderRiego() - ejecuta el comando LUCES ON | OFF | PROG
 *-----------------------------------------------------------------------*/
void atenderRiego(char *hacer, int hora, int minutos, int duracion, char *respuesta ){
	tConfig config;

	if (strcmp( hacer, "PROG" ) == 0) {			// PROG
		pthread_mutex_lock(&filelock);			//-----------------
		cargarConfig( &config );				//-----------------
		//config.luces = "LUCES";				//-----------------
		config.hriego = hora;					// Zona Critica
		config.mriego = minutos;				//-----------------
		config.driego = duracion;				//-----------------
		guardarConfig( &config );
        respuesta = "Riego programado";				//-----------------
		pthread_mutex_unlock(&filelock);		//-----------------
	}else if (strcmp(hacer, "ON") == 0) {		// ON
		if (ESTADO_RIEGO == 0) {
            ESTADO_RIEGO = 1;
            respuesta = "Riego encendido";
        }
        else {
            respuesta = "Riego ya se encuentra encendido";
        }
	} else if (strcmp(hacer, "OFF") == 0) {		// OFF
		if (ESTADO_RIEGO == 1) {
            ESTADO_RIEGO = 0;
            respuesta = "Riego apagado";
        }
        else {
            respuesta = "Riego ya se encuentra apagado";
        }  
    } 
}

void atenderImagen(){

}

void atenderLlamada(char *mensaje, int socketUDP, int recibido){
	
	printf("[%ld] %d Bytes recibidos, Comando: %s\n ",socketUDP, recibido, mensaje);
	
}
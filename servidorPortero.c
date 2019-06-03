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

void *atenderPeticionTCP( void *d ) {
	int total;	
	char msg[ MAXLINEA ];
	int sockTCP = (int)d;
	//msg = (char*)malloc(MAXLINEA);
	
   	printf( "(%d) Atendiendo petición.\n", sockTCP );
	while ( total = recibir( sockTCP, msg ) > 0 ) {
		printf( "(%d) Recibido: %s\n", sockTCP, msg );
		
		/*-------------------------------------------------------* 
		* Realizar la tarea específica del servicio
		*-------------------------------------------------------*/
		procesar( msg, sockTCP );
	
		/*-------------------------------------------------------* 
	 	* Responder petición		      					
		*-------------------------------------------------------*/
		if ( ( total = enviar( sockTCP, msg ) ) < 0 ) { 
			perror("(%d) ERROR ENVIAR TCP: ");
			exit(-1);
		}
		printf("(%d) Respuesta enviada: %s.\n", sockTCP, msg );
	}
}

void *atenderPeticionUDP( void *d ) {
	struct sockaddr_in dir_cli;
	int recibido;
	socklen_t longitud;
	char msg[ MAXLINEA ];
	int sockUDP = (int)d;

	for(;;) {
	 longitud = sizeof( dir_cli );
	 recibido = recvfrom( sockUDP, msg, MAXLINEA, 0, (struct sockaddr *) &dir_cli, &longitud );
	 //Codigo para atender UDP
	 //sendto( sockUDP, msg, recibido, 0, dir_cli_p, longitud );
	 printf("[%d] %d Bytes recibidos, Comando: %s\n ",sockUDP, recibido, msg);
	}
	
}

int main ( int argc, char *argv[] ) {
	int pid_hiloTCP, pid_hiloUDP;

	pid_hiloTCP=1000;
	pid_hiloUDP=5000;
	
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
	int opt = 1;  			//Opcion para reusar el puerto: setsockopt
	
	pthread_t t_hijoTCP;
	pthread_t t_hijoUDP;
	struct sockaddr_in dir_srv;
	
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
	if ( bind( sock_TCP, (struct sockaddr *) &dir_srv, sizeof( dir_srv ) ) < 0 ){
		printf("ERROR BIND TCP:\n");
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
	
	/*
	 * Comenzar tarea del servidor UDP
	 */
	pthread_create( &t_hijoUDP, NULL, atenderPeticionUDP, (void *)sock_UDP );
	/*
	 * Comenzar tarea del servidor TCP
	 */
	while ( 1 ) { 
		/*----------------------------------------------------------------* 
	 	 * Esperar una petición
	 	 *----------------------------------------------------------------*/
		printf( "Esperando petición (Padre).\n" );
		if ( ( ndescriptor = esperar(sock_TCP) ) < 0 ) {
			printf("ERROR ESPERANDO: ");
			exit( -3 );
		}
		
		pthread_create( &t_hijoTCP, NULL, atenderPeticionTCP, (void *)ndescriptor );

		//pthread_join( t_hijo, NULL );
	}

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
 	if (longitud > 0) 
		printf( "comando = %s\n", msg );
	return ( longitud );
}

/*-----------------------------------------------------------------------* 
 * procesar() - atender una petición
 *-----------------------------------------------------------------------*/
void procesar( char *mensaje, int socketTCP ) {
	tConfig config;
	char **palabras;
	int cp;

	printf ( "(%d) Procesando: %s \n", socketTCP, mensaje );
	cp = separarPalabras( mensaje, &palabras );
	if (strcmp( palabras[1], "PROG" ) == 0) {
		cargarConfig( &config );
		if (strcmp( palabras[0], "1") == 0) {
			//config.luces = "LUCES";
			config.hluces = atoi( palabras[2] );
			config.mluces = atoi( palabras[3] );
			config.dluces = atoi( palabras[4] );
		}
		else if (strcmp( palabras[0], "2") == 0) {
			//config.riego = "RIEGO";
			config.hriego = atoi( palabras[2] );
			config.mriego = atoi( palabras[3] );
			config.driego = atoi( palabras[4] );
		}
		guardarConfig( &config );
	}
 	
}
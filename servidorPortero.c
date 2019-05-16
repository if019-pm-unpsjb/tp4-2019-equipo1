#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>

#include "servidorPortero.h"

void *atenderPeticion( void *d ) {
	printf( "(%5d) Atendiendo petición.\n", getpid() );
	int total;	
	char *msg = (char*)malloc( sizeof( MAXLINEA + 1 ) );
		
   	while ( total = recibir( *((int *)d), msg ) > 0 ) {
		printf( "(%5d) Recibido: %s\n", getpid(), msg );
		
		/*-------------------------------------------------------* 
		* Realizar la tarea específica del servicio
		*-------------------------------------------------------*/
		procesar( msg );
	
		/*-------------------------------------------------------* 
	 	* Responder petición		      					
		*-------------------------------------------------------*/
		if ( ( total = enviar( *((int *)d), msg ) ) < 0 ) { 
			perror("ERROR ENVIAR: ");
			exit(-1);
		}
		printf("(%5d) Respuesta enviada: %s.\n", getpid(), msg );
	}
}

int main ( int argc, char *argv[] ) {

    //char *msg;
	int descriptor, ndescriptor, total, pid_hijo;
	pthread_t t_hijo;

	
	/*---------------------------------------------------------------------*
	 * Verificar los argumentos
	 *---------------------------------------------------------------------*/
 	if ( argc < 2 ) {
		printf( "Uso: servidor <puerto>\n" );
		exit( -1 );
	}
	printf( "\tServidor de eco concurrente sobre TCP. Para salir presione <Ctrl>-C\n" );
	
	/*--------------------------------------------------------------------* 
	 * Inicializar el servidor
	 *--------------------------------------------------------------------*/
	/* Tomar el puerto donde va a atender
	*/ 
	if ( ( descriptor = inicializar( atoi( argv[1] ) ) ) < 0 ) {
		perror( "ERROR INICIALIZAR: " );
		exit(-1);
	}	
	printf( "Servidor inicializado.\n" );

    //msg = (char*)malloc( sizeof( MAXLINEA + 1 ) );

	/*
	 * Comenzar tarea del servidor
	 */
	while ( 1 ) { 
		/*----------------------------------------------------------------* 
	 	 * Esperar una petición
	 	 *----------------------------------------------------------------*/
		printf( "Esperando petición (Padre).\n" );
		if ( ( ndescriptor = esperar( descriptor ) ) < 0 ) {
			perror("ERROR ESPERANDO: ");
			exit(-1);
		}
		
		pthread_create( &t_hijo, NULL, atenderPeticion, (void *)&ndescriptor );

		//pthread_join( t_hijo, NULL );


		/*-------------------------------------------------------*  
		 * Realizar fork del proceso para atender una petición 
		 *-------------------------------------------------------*/
		/*
		if ( ( pid_hijo = fork() ) < 0 ) {
			perror("ERROR FORK:");
			exit(-1);
   		} else if ( pid_hijo == 0 ) { 
			printf( "(%5d) Atendiendo petición.\n", getpid() );
			
   			while ( total = recibir( ndescriptor, msg ) > 0 ) {
				printf( "(%5d) Recibido: %s\n", getpid(), msg );
		
				procesar( msg );
	
				if ( ( total = enviar( ndescriptor, msg ) ) < 0 ) { 
					perror("ERROR ENVIAR: ");
					exit(-1);
				}
				printf("(%5d) Respuesta enviada: %s.\n", getpid(), msg );
			}

			close( ndescriptor );
			printf("(%5d) Conexión cerrada.\n", getpid() );
			exit( 0 );
		} else 
			printf( "Solicitud atendida por el proceso hijo nro. %d\n", pid_hijo );

		*/
	}

    return 1;
}



/*-------------------------------------------------------------------------*
 * inicializar() - inicializar el servidor
 *-------------------------------------------------------------------------*/ 	    
int inicializar( int puerto ) {
	int sockfd;
	struct sockaddr_in dir_srv;
	
	if ( ( sockfd = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
		return ( -1 );
	
	/* bind de la dirección local */
	bzero( (char *) &dir_srv, sizeof( dir_srv ) );
	dir_srv.sin_family = AF_INET;	/* utilizará familia de protocolos de Internet */
	dir_srv.sin_addr.s_addr = htonl( INADDR_ANY );		/* sobre la dirección IP local */
	dir_srv.sin_port = htons( puerto );

	if ( bind( sockfd, (struct sockaddr *) &dir_srv, sizeof( dir_srv ) ) < 0 )
		return ( -2 );
	
	/* armar una lista de espera para MAXCLI clientes */	
	listen( sockfd, MAXCLIENTES );
	
	/* mostrar un mensaje con los datos del socket de escucha.
	 * Sólo para ver qué está haciendo. Esta función no debería mostrar salida por pantalla.
	 */
	printf ( "\n\tEscuchando en puerto: %d, dirección %d. \n\tPodría tener hasta %d clientes esperando.\n", 
		ntohs( dir_srv.sin_port ), ntohl( dir_srv.sin_addr.s_addr ), MAXCLIENTES );
	
	return ( sockfd );
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
 	printf( "comando = %s\n", msg );
	return ( longitud );
}

/*-----------------------------------------------------------------------* 
 * procesar() - atender una petición
 *-----------------------------------------------------------------------*/
void procesar( char *mensaje ) {
	printf ( "(%5d) Procesando: %s \n", getpid(), mensaje );
}
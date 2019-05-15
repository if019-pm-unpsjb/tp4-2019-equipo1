#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

#include <string.h>
#include <stdlib.h>

#include "clientePortero.h"

main ( int argc, char *argv[] ) {

    int descriptor;
	struct sockaddr_in dir;

	/*---------------------------------------------------------------------
	 * Verificar los argumentos
	 *---------------------------------------------------------------------*/
 	if ( argc < 3 ) {
		printf( "Uso: cliente <direccion>\n" );
		printf( "     Donde: <direccion> = <ip> <puerto>\n" );
		exit( -1 );
	}

    printf( "\n\tCliente de Portero sobre TCP.\n" );
	
	/*-------------------------------------------------------------------- 
	 * Establecer la dirección del servidor y conectarse
	 *--------------------------------------------------------------------*/
    /**
	bzero( (char *) &dir, sizeof( dir ) );
	dir.sin_family = AF_INET;
	if ( inet_pton( AF_INET, argv[1], &dir.sin_addr ) <= 0 ) {
	    perror( "inet_pton" );
	    exit( -1 );
	}    
	dir.sin_port = htons( atoi( argv[2] ) );

	if ( ( descriptor = conectar( dir ) ) < 0 ) {
		perror( "ERROR CONECTAR:" );
		exit( -1 );
	}
    */
	
    principal( stdin, 0 );
	
	/*---------------------------------------------------------------------
	 * Cerrar la conexión y terminar
	 *---------------------------------------------------------------------*/
    /**
	cerrar( descriptor );
	printf( "Proceso cliente finalizado.\n" );
	exit( 0 );
    */

}

/*-------------------------------------------------------------------------
 * conectar() 
 *-------------------------------------------------------------------------*/ 	    
int conectar( struct sockaddr_in dir ) {
	int sockfd;
	
	/* abrir el socket TPC */
	if ( ( sockfd = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
		return ( -1 );
	
	/* conectarse al servidor */
	if ( connect( sockfd, (struct sockaddr *) &dir, sizeof( dir ) ) < 0 ) 
		return ( -2 );
	
	return ( sockfd );
}

int principal( FILE *fp, int sockfd ) {
	int resultado, total;
	char msg[ MAXLINEA ];
    char msg2[ MAXLINEA + 1 ];

    /*---------------------------------------------------------------------
	 * Comandos del cliente 
	 *---------------------------------------------------------------------*/
    prinf( "--- Comandos: \n1) Luces ON/OFF/PROG Hora Minuto Duracion\n2) Riego ON/OFF/PROG Hora Minuto Duracion\n3) Imagen Portero\n4) Contestar llamanda\n5) Salir");
	while( fgets( msg, MAXLINEA, fp ) != NULL ) {
		msg[ strlen( msg ) -1 ] = '\0';

        int i = analizar( msg );
        if (i < 0)
            exit(0);  
	}
}

int analizar( char *in ) {
    char sp[5] = " \t\n";
    char *ptr;
    char mens[20];

    ptr = strtok( in, sp );
    int len = strlen( ptr );
    
    int opt = atoi( ptr[0] );
    switch (opt)
    {
        case 1:
            if (strcmp( ptr[1], "ON") == 0) {
                // llamar a comando luces prender
                printf( "llamar a comando luces prender");
            }
            else if (strcmp( ptr[1], "OFF") == 0) {
                // llamar a comando luces apagar
                printf("llamar a comando luces apagar");
            }
            else if (strcmp( ptr[1], "PROG" ) == 0){
                // ingresa por PROG
                int h = atoi( ptr[2] );
                int m = atoi( ptr[3] );
                int d = atoi( ptr[4] );
                // llamar a comando programar luces
                printf("llamar a comando programar luces");
            }
            else {
                // comando invalido
                printf( "comando invalido ");
            }
            break;
        
        case 2:
            if (strcmp( ptr[1], "ON") == 0) {
                // llamar a comando luces prender
                printf( "llamar a comando riego prender");
            }
            else if (strcmp( ptr[1], "OFF") == 0) {
                // llamar a comando luces apagar
                printf("llamar a comando riego apagar");
            }
            else if (strcmp( ptr[1], "PROG" ) == 0){
                // ingresa por PROG
                int h = atoi( ptr[2] );
                int m = atoi( ptr[3] );
                int d = atoi( ptr[4] );
                // llamar a comando programar luces
                printf("llamar a comando programar riego");
            }
            else {
                // comando invalido
                printf( "comando invalido ");
            }
            break;
        
        case 3:
            printf( "Pedir Imagen");
            break;

        case 4: 
            printf( "contestar llamada" );
            break;
        
        case 5:
            printf( "Salir.....");
        default:
            printf( "Opcion Incorrecta" );
            break;
    }

    return -1;

}
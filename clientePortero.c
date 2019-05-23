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

/* cambios */

int main ( int argc, char *argv[] ) {

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

	
    principal( stdin, descriptor );
	
	/*---------------------------------------------------------------------
	 * Cerrar la conexión y terminar
	 *---------------------------------------------------------------------*/
    
    printf( "Cerrando conexion....\n");
	close( descriptor );
	printf( "Proceso cliente finalizado.\n" );
	exit( 0 );
    return 0;

}

/*-------------------------------------------------------------------------
 * conectar() 
 *-------------------------------------------------------------------------*/ 	    
int conectar( struct sockaddr_in dir ) {
	int sockfd;
	
	/* abrir el socket TPC */
    printf( "Abriendo socket\n");
	if ( ( sockfd = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
		return ( -1 );
	
	/* conectarse al servidor */
    printf( "conectando servidor\n");
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
    printf( "Comandos: \n1) Luces ON/OFF/PROG Hora Minuto Duracion\n2) Riego ON/OFF/PROG Hora Minuto Duracion\n3) Imagen Portero\n4) Contestar llamanda\n5) Salir\nIngrese opcion:  ");
	while( fgets( msg, MAXLINEA, fp ) != NULL ) {
		msg[ strlen( msg ) -1 ] = '\0';
        char **ptr;
        //separarPalabras( msg, ptr );
        //lo cambie para que el cliente se mantenga abierto
        if (resultado=analizar( msg, sockfd ) < 0)
            exit(0);  
	}
}

int analizar( char *in, int sockfd ) {
    
    char **ptr;
    int cp;

    cp = separarPalabras( in, &ptr );
    int opt = (int)atoi( ptr[0] );
    char *m = (char*)malloc( sizeof( MAXLINEA + 1 ) );
    switch (opt)
    {
        case 1:
            if (cp >= 2) {
                if (strcmp( ptr[1], "ON") == 0) {
                    // llamar a comando luces prender
                    printf( "llamar a comando luces prender\n");
                    enviar( sockfd, "LUCES ON", 8 );
                    recibirRespuesta( sockfd, m, (int)MAXLINEA);
                }
                else if (strcmp( ptr[1], "OFF") == 0) {
                    // llamar a comando luces apagar
                    printf("llamar a comando luces apagar\n");
                    enviar( sockfd, "LUCES OFF", 10 );
                    recibirRespuesta( sockfd, m,(int)MAXLINEA);
                }
                else if (strcmp( ptr[1], "PROG" ) == 0){
                    if (cp == 5) {
                        int h = atoi( ptr[2] );
                        int m = atoi( ptr[3] );
                        int d = atoi( ptr[4] );
                        // llamar a comando programar luces
                        printf("llamar a comando programar luces\n");
                        enviar( sockfd, "LUCES PROG ", 12 );
                        //recibirRespuesta( sockfd, m, (int)MAXLINEA);
                    }
                    else {
                        printf( "Luces PROG incorrecto\n");
                        opt = 6;
                    }
                }
                else {
                    // comando invalido
                    printf( "comando invalido \n");
                }
            }
            else {
                printf( "Error Luces\n");
            }
            break;
        
        case 2:
            if (strcmp( ptr[1], "ON") == 0) {
                // llamar a comando luces prender
                printf( "llamar a comando riego prender\n");
                enviar( sockfd, "RIEGO ON", 9 );
                recibirRespuesta( sockfd, m,(int)MAXLINEA);
            }
            else if (strcmp( ptr[1], "OFF") == 0) {
                // llamar a comando luces apagar
                printf("llamar a comando riego apagar\n");
                enviar( sockfd, "RIEGO OFF", 10 );
                recibirRespuesta( sockfd, m,(int)MAXLINEA);
            }
            else if (strcmp( ptr[1], "PROG" ) == 0){
                if (cp == 5) {
                    int h = atoi( ptr[2] );
                    int m = atoi( ptr[3] );
                    int d = atoi( ptr[4] );
                    // llamar a comando programar luces
                    printf("llamar a comando programar riego\n");
                    enviar( sockfd, "RIEGO PROG", 11 );
                    //recibirRespuesta( sockfd, m,(int)MAXLINEA);
                }
                else {
                    printf( "Riego PROG incorrecto\n");
                    opt = 6;
                }
                
            }
            else {
                // comando invalido
                printf( "comando invalido \n");
            }
            break;
        
        case 3:
            printf( "Pedir Imagen\n");
            break;

        case 4: 
            printf( "contestar llamada\n" );
            break;
        
        case 5:
            printf( "Salir.....\n");
            opt = -1;
            break;
        default:
            printf( "Opcion Incorrecta\n" );
            opt = 6;
            break;
    }

    return opt;

}

int enviar( int dcon, char *msg, int len ) {
    int result;

    if ((result = write( dcon, msg, len ) )< 0) 
        return -1;
    
    return result;
}

int recibirRespuesta( int dcon, char *msg, int len ) {
    int longitud;

	bzero( msg, MAXLINEA );

	if ( ( longitud =  read( dcon, msg, (int)MAXLINEA ) ) < 0 ) {
		return ( -2 );
	}
 	printf( "recibido = %s\n", msg );
	return ( longitud );
}


int separarPalabras( char *cadena, char ***aaargs ){
    //char cadena[] = "Esto es un texto. Puede ir separado por puntos, espacios o comas.",
    char delimitador[] = " \t\n";
    char **aargs;
    char *tmp;
    int num=0;
    int i;
    
    //Aca falla cuando analiza el 2do comando
    //comando = 
    //corrupted size vs. prev_size
    //Abortado (`core' generado)
    aargs=malloc(sizeof(char**));

    tmp = strtok(cadena, delimitador);
    
    do {
        aargs[num]=malloc(sizeof(char*));

        /*       strcpy(aargs[num], tmp); */
        aargs[num]=tmp;
        num++;

        /* Reservamos memoria para una palabra más */
        aargs=realloc(aargs, sizeof(char**)*(num+1));

        /* Extraemos la siguiente palabra */
        tmp = strtok(NULL, delimitador);
    } while (tmp!=NULL);  

    *aaargs = aargs;

    return num;
}
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

#include <string.h>
#include <stdlib.h>

#include "clientePortero.h"
#include "porteroUtils.h"

//int PUERTO_SRV = 0;
//char *ADDR_SRV;


int main ( int argc, char *argv[] ) {
    int sockTCP;
    int sockUDP;
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
	    perror("Error en la función inet_pton:");
	    exit( -1 );
	}    
	dir.sin_port = htons(atoi(argv[2]));

	// Obtengo el socket TCP
    if ( ( sockTCP = conectar( dir ) ) < 0 ) {
		perror( "ERROR CONECTAR TCP:" );
		exit( -1 );
	}

	/*---------------------------------------------------------------------*
	 * Realizar la función del cliente
	 *---------------------------------------------------------------------*/
	principal( stdin, sockTCP, argv);

	/*---------------------------------------------------------------------
	 * Cerrar la conexión TCP y terminar
	 *---------------------------------------------------------------------*/
    printf( "Cerrando conexion....\n");
	close( sockTCP );
	printf( "Proceso cliente finalizado.\n" );

    return 1;
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
    printf( "Conectando al servidor...\n");
	if ( connect( sockfd, (struct sockaddr *) &dir, sizeof( dir ) ) < 0 ) 
        return ( -2 );
	
	return ( sockfd );
}

/*-------------------------------------------------------------------------
 * principal() 
 *-------------------------------------------------------------------------*/ 	    
int principal( FILE *fp, int sockTCP, char *args[]) {
	int resultado, total;
	char msg[ MAXLINEA ];
    char respuesta[ MAXLINEA + 1 ];

    /*---------------------------------------------------------------------
	 * Comandos del cliente 
	 *---------------------------------------------------------------------*/
    system("clear");
    printf( "ADMINISTRADOR PORTERO - Comandos: \n--------------------------------\n1) Luces ON/OFF/PROG Hora Minuto Duracion\n2) Riego ON/OFF/PROG Hora Minuto Duracion\n3) Imagen Portero\n4) Contestar llamanda\n5) Salir\n\nIngrese opcion:  ");
	while( fgets( msg, MAXLINEA, fp ) != NULL ) {
		msg[ strlen( msg ) -1 ] = '\0';
        while (analizar( msg, sockTCP, respuesta, args) != 0)
            break; 
        printf( "\n\nPresione una tecla para continuar.... ");
        getchar();
        system("clear");
        printf( "ADMINISTRADOR PORTERO - Comandos: \n--------------------------------\n1) Luces ON/OFF/PROG Hora Minuto Duracion\n2) Riego ON/OFF/PROG Hora Minuto Duracion\n3) Imagen Portero\n4) Contestar llamanda\n5) Salir\n\nIngrese opcion:  ");
	}
}

int analizar( char *in, int sockfd, char *resp, char *args[]) {
    char **ptr;
    int cp;
    int ret=1;
    char cadenain[MAXLINEA];

    strcpy( cadenain, in );
    cp = separarPalabras( cadenain, &ptr );
    int opt = (int)atoi( ptr[0] );
    
    switch (opt)
    {
        case 1:
            if (cp >= 2) {
                if (strcmp( ptr[1], "ON") == 0) {
                    // llamar a comando luces prender
                    //printf( "llamar a comando luces prender cadena: %s, long:%ld \n", in, strlen( in));
                    enviar( sockfd, in, strlen( in ) );
                    recibirRespuesta( sockfd, resp, (int)MAXLINEA);
                }
                else if (strcmp( ptr[1], "OFF") == 0) {
                    // llamar a comando luces apagar
                    //printf("llamar a comando luces apagar\n");
                    enviar( sockfd, in, strlen( in ) );
                    recibirRespuesta( sockfd, resp,(int)MAXLINEA);
                }
                else if (strcmp( ptr[1], "PROG" ) == 0){
                    if (cp == 5) {
                        // llamar a comando programar luces
                        //printf("llamar a comando programar luces\n");
                        if (validarProgramacion( atoi( ptr[2] ),atoi( ptr[3] ),atoi( ptr[4] ) ) == -1) {
                            ret = 6;
                            printf( "Luces PROG: error valores hora minutos o duracion. Hora: 0-24, minutos: 0-60, duracion: 1-12");   
                        }
                        else {
                            enviar( sockfd, in, strlen( in ) );
                            recibirRespuesta( sockfd, resp, (int)MAXLINEA);
                        }
                    }
                    else {
                        printf( "Luces PROG incorrecto\nUso: 1 PROG hora minutos duracion");
                        ret = 6;
                    }
                }
                else {
                    // comando invalido
                    printf( "Error Luces\nUso: 1 [OFF|ON|PROG hora minutos duracion] ");
                }
            }
            else {
                printf( "Error Luces\nUso: 1 [OFF|ON|PROG hora minutos duracion] ");
            }
            break;
        
        case 2:
            if (cp >= 2) {
                if (strcmp( ptr[1], "ON") == 0) {
                    // llamar a comando luces prender
                    //printf( "llamar a comando riego prender\n");
                    enviar( sockfd, in, strlen( in ) );
                    recibirRespuesta( sockfd, resp,(int)MAXLINEA);
                }
                else if (strcmp( ptr[1], "OFF") == 0) {
                    // llamar a comando luces apagar
                    //printf("llamar a comando riego apagar\n");
                    enviar( sockfd, in, strlen( in ) );
                    recibirRespuesta( sockfd, resp,(int)MAXLINEA);
                }
                else if (strcmp( ptr[1], "PROG" ) == 0){
                    if (cp == 5) {
                        // llamar a comando programar luces
                        //printf("llamar a comando programar riego\n");
                        if (validarProgramacion( atoi( ptr[2] ),atoi( ptr[3] ),atoi( ptr[4] ) ) == -1) {
                            ret = 6;
                            printf( "Riego PROG: error valores hora minutos o duracion. Hora: 0-24, minutos: 0-60, duracion: 1-12");   
                        }
                        else {
                            enviar( sockfd, in, strlen( in ) );
                            recibirRespuesta( sockfd, resp,(int)MAXLINEA);
                        }
                    }
                    else {
                        printf( "Riego PROG incorrecto\nUso: 2 PROG hora minutos duracion");
                        ret = 6;
                    } 
                }
                else {
                    // comando invalido
                    printf( "Error Riego\nUso: 2 [OFF|ON|PROG hora minutos duracion] ");
                }
            }
            else {
                printf( "Error Riego\nUso: 2 [OFF|ON|PROG hora minutos duracion] ");
            }
            break;
        
        case 3:
            printf( "Pedir Imagen\n");
            enviar( sockfd, in, strlen( in ) );
            recibirRespuesta( sockfd, resp,(int)MAXLINEA);
            break;

        case 4: 
            contestarLlamada(args);
            break;
        case 5:
            printf( "Salir.....\n");
            ret = -1;
            break;
        default:
            printf( "Opcion Incorrecta\n" );
            ret = 6;
            break;
    }

    return ret;

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
 	printf( "\nrecibido = %s\n", msg );
	return ( longitud );
}

int validarProgramacion( int horas, int minutos, int duracion ) {
    if (horas < 0 || horas > 24 || minutos < 0 || minutos > 60 || duracion < 1 || duracion > 12 )
        return -1;
    return 1;
}

void contestarLlamada(char *args[]){
	int sockfd; 
	char buffer[MAXLINEA]; 
	struct sockaddr_in	 servaddr; 

    //if (argc < 3) {
    //    fprintf(stderr, "Use: %s ip port\n", args[0]);
    //    exit(EXIT_FAILURE);
    //}

	// Create socket 
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
		perror("socket creation failed"); 
		exit(EXIT_FAILURE); 
	} 

	memset(&servaddr, 0, sizeof(servaddr)); 
	
	// Server address
	servaddr.sin_family = AF_INET; 
    servaddr.sin_port = htons(atoi(args[2]));
    inet_aton(args[1], &(servaddr.sin_addr));
	
	int n, len; 
    //printf("Send data to server %s:%d ...\n", inet_ntoa(servaddr.sin_addr), ntohs(servaddr.sin_port));
	
    len = sizeof(servaddr);
    //n = sendto(sockfd, "",strlen(""), 0, (struct sockaddr*) &servaddr, sizeof(servaddr));
	FILE *fdclient;

    fdclient=fopen("/home/mcoppa/tp4/llamada/cliente1", "r");
	if ( fdclient == NULL){
		perror("Llamada: ");
        exit(EXIT_FAILURE);
	}
	// Comienzo la llamada
	while (!feof(fdclient)){
        bzero( buffer, MAXLINEA );
        fgets(buffer, MAXLINEA, fdclient);
		printf("[CLIENTE]: %s", buffer);
        sleep(1);
		// Lo envio al cliente para que tambien imprima la llamada
        n = sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr*) &servaddr, sizeof(servaddr));
		if (n < 0) {
            perror("sendto");
            exit(EXIT_FAILURE);
        }
        bzero( buffer, MAXLINEA );
        n = recvfrom(sockfd, (char *)buffer, MAXLINEA, 0, (struct sockaddr *) &servaddr, &len); 
       	if (n < 0) {
       	    perror("recvfrom");
			exit(EXIT_FAILURE);
       	}
		buffer[n]='\0';
		printf("[SERVER] [%s:%d] %s\n", inet_ntoa(servaddr.sin_addr), ntohs(servaddr.sin_port), buffer);
		sleep(1);
	
	}
	fclose(fdclient);
}
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
#include "porteroUtils.h"

/* cambios */

int main( int argc, char *argv[] ) {

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

    return 1;
}

/*-------------------------------------------------------------------------
 * conectar() 
 *-------------------------------------------------------------------------*/ 	    
int conectar( struct sockaddr_in dir ) {
	int sockfd;
	
	/* abrir el socket TPC */
    printf( "Abriendo socket\n");
	if ( (sockfd = socket( AF_INET, SOCK_STREAM, 0 )) < 0 )
		return ( -1 );
	
	/* conectarse al servidor */
    printf( "conectando servidor\n");
	if ( connect( sockfd, (struct sockaddr *) &dir, sizeof( dir ) ) < 0 ) 
		return ( -2 );
	
	return ( sockfd );
}


/*-------------------------------------------------------------------------
 * principal() 
 *-------------------------------------------------------------------------*/ 
int principal( FILE *fp, int sockfd ) {
	int resultado, total;
	char msg[ MAXLINEA ];
    char respuesta[ MAXLINEA ];
    bzero( respuesta, MAXLINEA );
    /*---------------------------------------------------------------------
	 * Comandos del cliente 
	 *---------------------------------------------------------------------*/
    system("clear");
    printf( "ADMINISTRADOR PORTERO - Comandos: \n--------------------------------\n1) Luces ON/OFF/PROG Hora Minuto Duracion\n2) Riego ON/OFF/PROG Hora Minuto Duracion\n3) Imagen Portero\n4) Contestar llamanda\n5) Salir\n\nIngrese opcion:  ");
	while( fgets( msg, MAXLINEA, fp ) != NULL ) {
		msg[ strlen( msg ) -1 ] = '\0';
        bzero( respuesta, MAXLINEA );
        if (analizar( msg, sockfd, respuesta ) == -1)
            break; 
        printf( "\n\nPresione una tecla para continuar.... ");
        getchar();
        system("clear");
        printf( "ADMINISTRADOR PORTERO - Comandos: \n--------------------------------\n1) Luces ON/OFF/PROG Hora Minuto Duracion\n2) Riego ON/OFF/PROG Hora Minuto Duracion\n3) Imagen Portero\n4) Contestar llamanda\n5) Salir\n\nIngrese opcion:  ");
	}
}

/*-------------------------------------------------------------------------
 * analizar() 
 *-------------------------------------------------------------------------*/ 
int analizar( char *in, int sockfd, char *respuesta ) {
    char **ptr;
    int cp;
    int ret=0;
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
                    enviar( sockfd, in, strlen( in ) );
                    recibirRespuesta( sockfd, respuesta, (int)MAXLINEA);
                }
                else if (strcmp( ptr[1], "OFF") == 0) {
                    // llamar a comando luces apagar
                    enviar( sockfd, in, strlen( in ) );
                    recibirRespuesta( sockfd, respuesta,(int)MAXLINEA);
                }
                else if (strcmp( ptr[1], "PROG" ) == 0){
                    if (cp == 5) {
                        // llamar a comando programar luces
                        if (validarProgramacion( atoi( ptr[2] ),atoi( ptr[3] ),atoi( ptr[4] ) ) == -1) {
                            ret = 6;
                            printf( "Luces PROG: error valores hora minutos o duracion. Hora: 0-24, minutos: 0-60, duracion: 1-12");   
                        }
                        else {
                            enviar( sockfd, in, strlen( in ) );
                            recibirRespuesta( sockfd, respuesta, (int)MAXLINEA);
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
                    enviar( sockfd, in, strlen( in ) );
                    recibirRespuesta( sockfd, respuesta,(int)MAXLINEA);
                }
                else if (strcmp( ptr[1], "OFF") == 0) {
                    // llamar a comando luces apagar
                    enviar( sockfd, in, strlen( in ) );
                    recibirRespuesta( sockfd, respuesta,(int)MAXLINEA);
                }
                else if (strcmp( ptr[1], "PROG" ) == 0){
                    if (cp == 5) {
                        // llamar a comando programar luces
                        if (validarProgramacion( atoi( ptr[2] ),atoi( ptr[3] ),atoi( ptr[4] ) ) == -1) {
                            ret = 6;
                            printf( "Riego PROG: error valores hora minutos o duracion. Hora: 0-24, minutos: 0-60, duracion: 1-12");   
                        }
                        else {
                            enviar( sockfd, in, strlen( in ) );
                            recibirRespuesta( sockfd, respuesta,(int)MAXLINEA);
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
            procesarTransferencia( sockfd, respuesta );
            //recibirRespuesta( sockfd, respuesta,(int)MAXLINEA);
            break;

        case 4: 
            printf( "contestar llamada\n" );
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

/*-------------------------------------------------------------------------
 * enviar() 
 *-------------------------------------------------------------------------*/ 
int enviar( int dcon, char *msg, int len ) {
    int result;
    if ((result = write( dcon, msg, len ) )< 0) 
        return -1;
    return result;
}


/*-------------------------------------------------------------------------
 * recibirRespuesta() 
 *-------------------------------------------------------------------------*/ 
int recibirRespuesta( int dcon, char *respuesta, int len ) {
    int longitud;

	if ((longitud = read( dcon, respuesta, len )) < 0) {
		return ( -2 );
	}
    printf( "\nrecibido = %s\n", respuesta );
	return ( longitud );
}


/*---------------------------------------------------------------------------------------------------
 * recibirTransferencia(): gestiona la tranferencia de una imagen y almacena localmente en un archivo
 *--------------------------------------------------------------------------------------------------*/ 
int procesarTransferencia( int sock, char *resp ) {
    int ack;
    int num_blks, num_last_blk;
    FILE *fp;
    char in_buf[MAXSIZE];
    int no_writen;
    int i;
    char filename[] = "./recibido.jpg";
    ack = ACK;  
    ack = htons( ack);

    printf("Cliente: abriendo/creando archivo local: %s....\n", filename );
    if ((fp = fopen( filename,"w+")) == NULL) { 
        printf(" client: local open file error \n");
        return -1;
    }

    /* Espera recibir informacion desde el servidor, cantidad de bloques de tamaño MASIZE
       y cantidad de bytes del ultimo bloque variable */
    printf("Cliente: recibiendo mensaje de cantidad de blokes.....\n" );
    if((readn( sock,(char *)&num_blks,sizeof(num_blks) )) < 0) { 
        printf( "client: read error on nblocks :%d\n",errno );
        return -1;
    }
    num_blks = ntohs( num_blks );
    printf( "client: server responded: %d blocks in file......\n",num_blks );
    ack = ACK;  
    ack = htons( ack);
    printf("Cliente: enviando ACK cantidad de blokes.......\n" );
    if((writen( sock,(char *)&ack,sizeof(ack) )) < 0) {
        printf("client: ack write error :%d\n",errno);
        return -1;
    }

    printf("Cliente: recibiendo mensaje de cantidad de bytes del ultimo bloke........\n" );
    if((readn( sock,(char *)&num_last_blk,sizeof(num_last_blk) )) < 0) {
        printf("client: read error :%d on nbytes\n",errno);
        return -1;
    }

    num_last_blk = ntohs(num_last_blk);  
    printf("client: server responded: %d bytes last blk\n",num_last_blk );
    printf("Cliente: enviando ACK de cantidad de bytes del ultimo bloke.........\n" );
    if((writen(sock,(char *)&ack,sizeof(ack))) < 0) {
        printf("client: ack write error :%d\n",errno);
        return -1;
    }
    
    /* Comienza a recibir bloques de datos desde el servidor */
    printf("client: starting to get file contents..........\n");
    for(i= 0; i < num_blks; i ++) {
        printf("Cliente: recibiendo transferencia bloque %d...........\n", i );
        if((readn( sock,in_buf,MAXSIZE )) < 0) {
            printf("client: block error read: %d\n",errno); 
            return -1;
        }
        printf("Cliente: escribiendo blocke %i en archivo local...........\n", i );
        no_writen = fwrite( in_buf,sizeof(char),MAXSIZE,fp );
        if (no_writen < 0)
            printf("Cliente: menor que 0");
        if (no_writen == 0) {
            printf("client: file write error\n");
            return -1;
        }
        if (no_writen != MAXSIZE) { 
            printf("client: file write  error : no_writen is less\n");
            return -1;
        }
        /* envia ACK de el bloque recibido */
        printf("Cliente: enviando ACK de bloque %d a servidor...........\n", i );
        if((writen( sock,(char *)&ack,sizeof(ack) )) < 0) {
            printf("client: ack write  error :%d\n",errno);
            return -1;
        }
    }

    /* Si hay un ultimo bloque con datos parciales, lo recibe */
    printf("Cliente: recibiendo ultimo bloque............\n" );
    if (num_last_blk > 0) {
        printf("%d\n",num_blks);      
        if((readn( sock,in_buf,num_last_blk )) < 0) {
            printf("client: last block error read :%d\n",errno);
            return -1;
        }
        printf("Cliente: escribiendo ultimo blocke en el archivo local............\n" );
        no_writen = fwrite( in_buf,sizeof(char),num_last_blk,fp ); 
        if (no_writen == 0) {
            printf("client: last block file write err :%d\n",errno);
            return -1;
        }
        if (no_writen != num_last_blk) {
            printf("client: file write error : no_writen is less 2\n");
            return -1;
        }
        if((writen( sock,(char *)&ack,sizeof(ack) )) < 0) {
            printf("client :ack write  error  :%d\n",errno);
            return -1;
        }
    }
    else 
        printf("\n");
      
    printf("client: FILE TRANSFER COMPLETE.............\n");

    if (read( sock, resp, MAXLINEA ) < 0) {
		return ( -2 );
	}
    printf( "\nrecibido = %s\n", resp );

    /* Cierra archivo local y termina */
    fclose(fp);
    return 1;
}

/* DUE TO THE FACT THAT BUFFER LIMITS IN KERNEL FOR THE SOCKET MAY BE 
   REACHED, IT IS POSSIBLE THAT READ AND WRITE MAY RETURN A POSITIVE VALUE
   LESS THAN THE NUMBER REQUESTED. HENCE WE CALL THE TWO PROCEDURES
   BELOW TO TAKE CARE OF SUCH EXIGENCIES */

/*-------------------------------------------------------------------------
 * readn() 
 *-------------------------------------------------------------------------*/ 
int readn(int sd,char *ptr,int size) {         
    int no_left,no_read;
    no_left = size;
    
    while (no_left > 0) { 
        no_read = read(sd,ptr,no_left);
        if(no_read <0)  
            return(no_read);
        if (no_read == 0) 
            break;
        no_left -= no_read;
        ptr += no_read;
    }
    return(size - no_left);
}

/*-------------------------------------------------------------------------
 * writen() 
 *-------------------------------------------------------------------------*/ 
int writen(int sd,char *ptr,int size) {
    int no_left,no_written;
    no_left = size;
    
    while (no_left > 0) { 
        no_written = write(sd,ptr,no_left);
        if(no_written <=0)  
            return(no_written);
        no_left -= no_written;
        ptr += no_written;
    }
    return(size - no_left);
}

/*-------------------------------------------------------------------------
 * validarProgramacion() 
 *-------------------------------------------------------------------------*/ 
int validarProgramacion( int horas, int minutos, int duracion ) {
    if (horas < 0 || horas > 24 || minutos < 0 || minutos > 60 || duracion < 1 || duracion > 12 )
        return -1;
    return 1;
}
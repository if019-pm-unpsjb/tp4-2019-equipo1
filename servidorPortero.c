#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/ip.h>

#include "servidorPortero.h"
#include "porteroUtils.h"

static pthread_mutex_t filelock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t comm = PTHREAD_MUTEX_INITIALIZER;
static int ESTADO_LUCES = 0;
static int ESTADO_RIEGO = 0;
//int hilo =0;

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

#define BUF_SIZE 1024

int thread_cnt = 0;
struct in_addr ip_srv;

struct conn_info {
    int thread_id;
    int sock;
    struct sockaddr_in addr;
};


// Atiendo la llamada de cada cliente (hijo)
static void* atenderLlamada_udp(void* s){
    ssize_t n;
    char buf[BUF_SIZE];
    socklen_t len = sizeof(struct sockaddr_in);

    struct conn_info *client = (struct conn_info *) s;

    struct sockaddr_in naddr;
    memset(&naddr, 0, sizeof(struct sockaddr_in));
    naddr.sin_family = AF_INET;
    naddr.sin_port = htons(0);
    naddr.sin_addr.s_addr = ip_srv.s_addr;

    client->sock = socket(AF_INET, SOCK_DGRAM, 0);

    int optval = 1;
    if (setsockopt(client->sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &optval, sizeof(optval)) == -1) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    if (bind(client->sock, (struct sockaddr*) &naddr, sizeof(struct sockaddr_in)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    getsockname(client->sock, (struct sockaddr*) &naddr, &len);

    //printf("[Thread %d] I'm listening at port %d, and handling client %s:%d ...\n", client->thread_id, ntohs(naddr.sin_port), inet_ntoa((client->addr).sin_addr), ntohs((client->addr).sin_port));

    //Llamo al cliente cuando se conecta
	strcpy(buf, "[SERVER]: Hola!\n");
	//printf("%s\n", buf);
    sendto(client->sock, buf, strlen(buf), 0, (struct sockaddr*) &(client->addr), sizeof(struct sockaddr_in));

	FILE *fdserver;
	
	fdserver=fopen("/home/mcoppa/tp4/llamada/servidor1", "r");
	if ( fdserver == NULL){
		perror("Llamada: ");
        exit(EXIT_FAILURE);
	}
	// Comienzo la llamada
	while (!feof(fdserver)){
		bzero( buf, BUF_SIZE );
		n = recvfrom(client->sock, buf, BUF_SIZE, 0, (struct sockaddr*) &naddr, &len);
       	if (n < 0) {
       	    perror("recvfrom");
			exit(EXIT_FAILURE);
       	}
		//buf[n]='\0';
		//printf("[CLIENTE %d] [%s:%d] %s\n", client->thread_id, inet_ntoa(naddr.sin_addr), ntohs(naddr.sin_port), buf);
		
		bzero( buf, BUF_SIZE );
		fgets(buf, BUF_SIZE, fdserver);
		//printf("[SERVER]: %s", buf);
		// Lo envio al cliente para que tambien imprima la llamada
		n = sendto(client->sock, buf, strlen(buf), 0, (struct sockaddr*) &(client->addr), sizeof(struct sockaddr_in));
		if (n < 0) {
            perror("sendto");
            exit(EXIT_FAILURE);
        }
	}
	fclose(fdserver);

    printf("[CLIENTE] [%s:%d] Llamada finalizada.\n", inet_ntoa((client->addr).sin_addr), ntohs((client->addr).sin_port));
    
    pthread_exit(0);
}

/*********************************************************************
 * iniciarServidorUDP()
 * *******************************************************************/
void *iniciarServidorUDP( void *p ) {
    int sock;
    struct sockaddr_in addr;
    struct conn_info *client;
    char buf[100];
	int puerto = (int)p;
 
    inet_aton("127.0.0.1", &ip_srv);

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(puerto);
    //addr.sin_addr.s_addr = htonl( INADDR_ANY );		// sobre la dirección IP local
	addr.sin_addr.s_addr = ip_srv.s_addr;

    if (bind(sock, (struct sockaddr*) &addr, sizeof(struct sockaddr_in)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    pthread_t thread;


    int n;
    for (;;) {
        socklen_t clientlen = sizeof(struct sockaddr_in);
        client = (struct conn_info*) malloc(sizeof(struct conn_info));
        memset(client, 0, sizeof(struct conn_info));
        
		bzero( buf, BUF_SIZE );
		n = recvfrom(sock, buf, sizeof(buf), 0, (struct sockaddr*) &(client->addr), &clientlen);

        printf("[CLIENTE] [%s:%d] Iniciando Llamada\n", inet_ntoa((client->addr).sin_addr), ntohs((client->addr).sin_port));
		buf[n] = '\0';

        client->thread_id = thread_cnt++;

        pthread_create(&thread, NULL, atenderLlamada_udp, (void*) client);
	}
}


int main ( int argc, char *argv[] ) {
	/*---------------------------------------------------------------------*
	 * Verificar los argumentos
	 *---------------------------------------------------------------------*/
     if (argc < 2) {
        fprintf(stderr, "Uso: %s puerto\n", argv[0]);
        exit(EXIT_FAILURE);
    }
	printf( "\tServidor Portero. Para salir presione <Ctrl>-C\n" );
	
	/*--------------------------------------------------------------------* 
	 * Inicializar el servidor
	 *--------------------------------------------------------------------*/
	if ( ( inicializar( atoi( argv[1] ) ) ) < 0 ) {
		perror( "ERROR INICIALIZAR: " );
		exit(-1);
	}	
}



/*-------------------------------------------------------------------------*
 * inicializar() - inicializar el servidor
 *-------------------------------------------------------------------------*/ 	    
int inicializar( int puerto ) {
	int sock_TCP;
	int ndescriptor;
	int opt = 1;  			//Opcion para reusar el puerto: setsockopt
	
	pthread_t t_hijoTCP;	//Estructura para atenderPeticionTCP	
	pthread_t t_hijoUDP;	//Estructura para atenderPeticionUDP
	struct sockaddr_in dir_srv;
	
	/*--------------------------------------------------------------------* 
	 * Inicializar el servidor TCP                          			  *
	 *--------------------------------------------------------------------*/
	if ( ( sock_TCP = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 ) {
		printf("ERROR SOCKET TCP:\n");
		exit ( -1 );
	}

	/* bind de la dirección local */
	bzero( (char *) &dir_srv, sizeof( dir_srv ) );
	dir_srv.sin_family = AF_INET;	/* utilizará familia de protocolos de Internet */
	dir_srv.sin_addr.s_addr = htonl( INADDR_ANY );		/* sobre la dirección IP local */
	dir_srv.sin_port = htons( puerto );

	setsockopt( sock_TCP, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof( opt ));

	// bind TCP
    int b;
	if ((b=bind( sock_TCP, (struct sockaddr *) &dir_srv, sizeof( dir_srv ) )) < 0 ){
		printf("ERROR BIND TCP:%d\n", b);
		exit ( -2 );
	}

	/* armar una lista de espera para MAXCLIENTES clientes */	
	listen( sock_TCP, MAXCLIENTES );
	
	printf ( "\n\tEscuchando en puerto: %d, dirección %d. \n\tPodría tener hasta %d clientes esperando.\n", ntohs( dir_srv.sin_port ), ntohl( dir_srv.sin_addr.s_addr ), MAXCLIENTES );
	printf( "Servidor inicializado.\n" );
	
	/*----------------------------------------------------------------* 
	 * Comenzar tarea del servidor UDP
	 *----------------------------------------------------------------*/
	pthread_create( &t_hijoUDP, NULL, iniciarServidorUDP, (void *)puerto );
	/*----------------------------------------------------------------*
	 * Comenzar tarea del servidor TCP
	 *----------------------------------------------------------------*/
	for(;;) { 
		/* 
	 	 * Esperar una petición
	 	 */
		//printf( "Esperando petición (Padre).\n" );
		if ( ( ndescriptor = esperar(sock_TCP) ) < 0 ) {
			printf("ERROR ESPERANDO: ");
			exit( -3 );
		}
		
		pthread_create( &t_hijoTCP, NULL, atenderPeticionTCP, (void *)ndescriptor );
	}
    pthread_mutex_destroy( &filelock );
	pthread_mutex_destroy( &comm );
	return ( 1 );
}

/*-------------------------------------------------------------------------*
 * esperar()
 *-------------------------------------------------------------------------*/ 	    
int esperar( int sockfd ) {
	int nsockfd;
    struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);

	/* aceptar una conexión */
	if ( ( nsockfd = accept( sockfd, &addr,&addrlen ) ) < 0 )
		return ( -1 );
	//printf("Atendiendo cliente: %d:%d\n", ntohl( addr.sin_addr.s_addr),ntohs( addr.sin_port ) );
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
			//atenderImagen());
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
        strcpy( respuesta, "OK: Luces Programadas" );		//-----------------
		pthread_mutex_unlock(&filelock);		//-----------------
	} else if (strcmp(hacer, "ON") == 0) {		// ON
		if (ESTADO_LUCES == 0) {
            ESTADO_LUCES = 1;
            strcpy( respuesta,"OK: Luces encendidas" );
        }
        else {
            strcpy( respuesta,"OK: Luces ya se encuentran encendidas" );
        }
	} else if (strcmp(hacer, "OFF") == 0) {		// OFF
		if (ESTADO_LUCES == 1) {
            ESTADO_LUCES = 0;
            strcpy( respuesta,"OK: Luces apagadas" );
        }
        else {
            strcpy( respuesta,"OK: Luces ya se encuentran apagadas" );
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
        strcpy( respuesta,"Riego programado" );				//-----------------
		pthread_mutex_unlock(&filelock);		//-----------------
	}else if (strcmp(hacer, "ON") == 0) {		// ON
		if (ESTADO_RIEGO == 0) {
            ESTADO_RIEGO = 1;
            strcpy( respuesta,"OK: Riego encendido" );
        }
        else {
            strcpy( respuesta, "OK: Riego ya se encuentra encendido" );
        }
	} else if (strcmp(hacer, "OFF") == 0) {		// OFF
		if (ESTADO_RIEGO == 1) {
            ESTADO_RIEGO = 0;
            strcpy( respuesta,"OK: Riego apagado" );
        }
        else {
            strcpy( respuesta,"OK: Riego ya se encuentra apagado" );
        }  
    } 
}

void atenderImagen(int stcp){

}

void atenderLlamada(char *mensaje, int socketUDP, int recibido){
	printf("[%d] %d Bytes recibidos, Comando: %s\n ",socketUDP, recibido, mensaje);
	
}
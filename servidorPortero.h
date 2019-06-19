#define MAXCLIENTES 10
#define MAXLINEA 200
#define MAXSIZE 512

#define ACK                   2
#define NACK                  3
#define TRANSFER              100
#define ERROR                 400


void procesarTCP( char *mensaje, int socketTCP );
int inicializar( int puerto );
int esperar( int desc_con );
int enviar( int desc_con, char *msg );
int recibir( int desc_con, char *msg );

void atenderLuces(char *hacer, int hora, int minutos, int duracion, char *respuesta );
void atenderRiego(char *hacer, int hora, int minutos, int duracion, char *respuesta );
void atenderPortero();
int atenderImagen( int stcp, char *mensaje );
int writen(int sd,char *ptr,int size);
int readn(int sd,char *ptr,int size);
void atenderLlamada(char *mensaje, int socketUDP, int recibido);

/* ************************************************************
* ******** COMANDOS 
* *************************************************************/
typedef enum {NA,LUCES,RIEGO,IMAGEN,LLAMADA,EXIT} tipoComando;

#define MAXCLIENTES 10
#define MAXLINEA 50


void procesarTCP( char *mensaje, int socketTCP );
void procesarUDP( char *mensaje, int socketUDP, int recibido );
int inicializar( int puerto );
int esperar( int desc_con );
int enviar( int desc_con, char *msg );
int recibir( int desc_con, char *msg );

void atenderLuces(char *hacer, int hora, int minutos, int duracion, char *respuesta );
void atenderRiego(char *hacer, int hora, int minutos, int duracion, char *respuesta );
void atenderPortero();
void atenderImagen();
void atenderLlamada(char *mensaje, int socketUDP, int recibido);

/* ************************************************************
* ******** COMANDOS 
* *************************************************************/
typedef enum {NA,LUCES,RIEGO,IMAGEN,LLAMADA,EXIT} tipoComando;

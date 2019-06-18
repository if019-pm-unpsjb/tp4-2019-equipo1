#define MAXLINEA 200

#define ACK                   2
#define NACK                  3
#define TRANSFER              100
#define ERROR                 400
#define MAXSIZE               512

int conectar( struct sockaddr_in dir );
int principal( FILE *fp, int dir );
int analizar( char *s, int sockfd,char *respuesta );
void luces( int enc_apag, int hora, int minutos );
void riego( int enc_apag, int hora, int minutos );
void solicitarImagen();
void contestarLlamada(int sockUDP,const struct sockaddr *dirUDP, socklen_t saUDP);
int recibirRespuesta( int sock, char *m, int len );
int enviar( int sock, char *m, int len );

int procesarTransferencia( int sock, char *resp );
int writen(int sd,char *ptr,int size);
int readn(int sd,char *ptr,int size);
int validarProgramacion( int hora, int minutos, int duracion );
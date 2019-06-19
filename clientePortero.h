#define MAXLINEA 200

#define ACK                   2
#define NACK                  3
#define TRANSFER              100
#define ERROR                 400
#define MAXSIZE               512

int principal( FILE *fp, int sockTCP, char *args[]);

int analizar( char *s, int sockfd, char *respuesta, char *args[]);
int conectar( struct sockaddr_in dir );

void luces( int enc_apag, int hora, int minutos );
void riego( int enc_apag, int hora, int minutos );
void solicitarImagen();
void contestarLlamada(char *args[]);
int recibirRespuesta( int sock, char *m, int len );
int enviar( int sock, char *m, int len );

int procesarTransferencia( int sock, char *resp );
int writen(int sd,char *ptr,int size);
int readn(int sd,char *ptr,int size);
int validarProgramacion( int hora, int minutos, int duracion );
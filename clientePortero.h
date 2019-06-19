#define MAXLINEA 1024

int conectar( struct sockaddr_in dir );

int principal( FILE *fp, int sockTCP, char *args[]);

int analizar( char *s, int sockfd, char *respuesta, char *args[]);

void luces( int enc_apag, int hora, int minutos );

void riego( int enc_apag, int hora, int minutos );

void solicitarImagen();

void contestarLlamada(char *args[]);

int separarPalabras( char *cad, char ***ptr );

int recibirRespuesta( int sock, char *m, int len );

int enviar( int sock, char *m, int len );

int validarProgramacion( int hora, int minutos, int duracion );
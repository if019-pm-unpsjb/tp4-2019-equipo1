#define MAXLINEA 50

int conectar( struct sockaddr_in dir );

int principal( FILE *fp, int dir );

int analizar( char *s, int sockfd );

void luces( int enc_apag, int hora, int minutos );

void riego( int enc_apag, int hora, int minutos );

void solicitarImagen();

void contestar( int fd_audio );

int separarPalabras( char *cad, char ***ptr );

int recibirRespuesta( int sock, char *m, int len );

int enviar( int sock, char *m, int len );

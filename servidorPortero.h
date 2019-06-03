#define MAXCLIENTES 10
#define MAXLINEA 50


void procesar( char *mensaje, int socketTCP );
int inicializar( int puerto );
int esperar( int desc_con );
int enviar( int desc_con, char *msg );
int recibir( int desc_con, char *msg );

void atenderLuces( );

void atenderRiego();

void atenderPortero();

void atenderImagen();

void atenderLlamda();
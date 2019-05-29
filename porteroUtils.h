#include <stdio.h>


typedef struct 
{
    char *luces;
    // programacion luces Hora Minutos Duración
    int hluces;
    int mluces;
    int dluces;
    char *riego; 
    // programacion riego Hora Minutos Duración
    int hriego;
    int mriego;
    int driego;
} tConfig;


// guardarConfig:
// Guarda los datos de la estructura en el archivo de configuracion
void guardarConfig(tConfig *config){
    FILE *fconfig;

    fconfig=fopen("config.cfg", "w+b");
    if (fconfig!=NULL){
        fwrite(config, sizeof(tConfig),1,fconfig);
    }
    fclose(fconfig);
}


// cargarConfig:
// Carga el contenido del archivo en la estructura
void cargarConfig(tConfig *config){
    FILE *fconfig;

    fconfig=fopen("config.cfg", "rb");
    if (fconfig!=NULL){
        fread(config, sizeof(tConfig), 1, fconfig);
    }
    fclose(fconfig);
}

//separarPalabras:
//Separa los comandos ingresados por linea de comandos según espacios, tabuladores
//y saltos de linea, devuelve la cantidad de palabras de la cadena 
int separarPalabras( char *cadena, char ***aaargs ){
    char delimitador[] = " \t\n";
    char **aargs;
    char *tmp;
    int num=0;
    int i;
    
    aargs=malloc(sizeof(char**));

    tmp = strtok(cadena, delimitador);
    do {
        aargs[num]=malloc(sizeof(char*));

        /*       strcpy(aargs[num], tmp); */
        aargs[num]=tmp;
        num++;

        /* Reservamos memoria para una palabra más */
        aargs=realloc(aargs, sizeof(char**)*(num+1));

        /* Extraemos la siguiente palabra */
        tmp = strtok(NULL, delimitador);
    } while (tmp!=NULL);  

    *aaargs = aargs;

    return num;
}
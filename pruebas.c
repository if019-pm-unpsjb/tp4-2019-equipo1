

#include "porteroUtils.h"

int main()
{
    char msg;
    tConfig prueba;
    tConfig *p = malloc( sizeof( tConfig ));

    printf("Cargar Archivo de configuración?\nS=carga\nN=graba datos por defecto.\n");
    msg=getchar();
    printf("getchar: %d.\n", msg);
    if (msg == 83) {
            printf("*************** Cargando configuracion....\n");
            cargarConfig( p );
            
            printf("LUCES: %d \n", p->hluces);
            /*
            printf("PROGRAMACION LUCES: \n");
            printf("%d:%d Duración %d minutos \n\n", prueba.hluces, prueba.mluces, prueba.dluces);
            
            printf("RIEGO: %s \n", prueba.riego);
            printf("PROGRAMACION RIEGO: \n");
            printf("%d:%d Duración %d minutos \n\n", prueba.hriego, prueba.mriego, prueba.driego);
            */
    }else
    {
        printf("*************** Guardando configuracion....\n");
        prueba.luces = "ON";
        prueba.hluces = 11;
        prueba.mluces = 44;
        prueba.dluces = 33;
        prueba.riego = "OFF";
        prueba.hriego = 22;
        prueba.mriego = 55;
        prueba.driego = 111;

        guardarConfig(&prueba);
    }
}
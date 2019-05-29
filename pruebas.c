#include <stdio.h>
#include "porteroUtils.h"


int main()
{
    char msg;
    tConfig prueba;

    printf("Cargar Archivo de configuración?\nS=carga\nN=graba datos por defecto.\n");

    if ( (msg=getchar()) == 'S') {
            cargarConfig(&prueba);
            
            printf("LUCES: %s \n", prueba.luces);
            printf("PROGRAMACION LUCES: \n");
            printf("%d:%d Duración %d minutos \n\n", prueba.hluces, prueba.mluces, prueba.dluces);
            
            printf("RIEGO: %s \n", prueba.riego);
            printf("PROGRAMACION RIEGO: \n");
            printf("%d:%d Duración %d minutos \n\n", prueba.hriego, prueba.mriego, prueba.driego);

    }else
    {
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
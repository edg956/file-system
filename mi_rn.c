/*
    PROGRAMA PARA CAMBIAR DE NOMBRE A UN FICHERO/DIRECTORIO 
*/

#include "bloques.h"
#include "directorios.h"
#include <string.h>

int main(int argc, char **argv) {

    char errbuff[BLOCKSIZE]; //Buffer de errores.
    memset(errbuff, 0, sizeof(errbuff));
    int nerror;

    //Comprobación de parámetros enviados al programa. 
    if (argc != 4) {
        fprintf(stderr, "Error: Sintaxis: /mi_rn <nombre_disco> /ruta_original /ruta_nueva/\n");
        exit(-1);
    }

    //Montaje del disco virtual. 
    if (bmount(argv[1]) == -1) {
        fprintf(stderr, "Error: no se ha podido abrir el directorio indicado.\n");
        exit(-1);
    }

    //Llamada a la función mi_rn. 
    if ((nerror = mi_rn(argv[2], argv[3])) < 0) {
        control_errores_buscar_entrada(nerror, errbuff);
        fprintf(stderr, "%s", errbuff);
        exit(-1);
    }

    //Desmontaje del dispositivo virtual. 
    if (bumount() == -1) {
        fprintf(stderr, "Error: no se ha podido cerrar el fichero.\n");
        exit(-1);
    }
}
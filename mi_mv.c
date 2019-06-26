/*
    PROGRAMA PARA MOVER UN FICHERO/DIRECTORIO DENTRO DE OTRO DIRECTORIO 
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
        fprintf(stderr, "Error: Sintaxis: /mi_mv <nombre_disco> /ruta_origen /ruta_destino/\n");
        exit(-1);
    }

    //Verificar que destino sea un directorio
    if (argv[3][strlen(argv[3]) - 1] != '/') {
        fprintf(stderr, "Error: ruta de destino ha de ser un directorio.\n");
        exit(-1);
    }

    //Montaje del disco virtual. 
    if (bmount(argv[1]) == -1) {
        fprintf(stderr, "Error: no se ha podido abrir el directorio indicado.\n");
        exit(-1);
    }

    //Llamada a la función mi_mv. 
    if ((nerror = mi_mv(argv[2], argv[3])) < 0) {
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
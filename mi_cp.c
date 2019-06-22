/*
    PROGRAMA PARA COPIAR UN FICHERO/DIRECTORIO DENTRO DE OTRO DIRECTORIO 
*/

#include "bloques.h"
#include "directorios.h"
#include <string.h>

int main(int argc, char **argv) {

    //Declaraciones
    char errbuff[BLOCKSIZE]; 

    //Inicialización del buffer de errores. 
    memset(errbuff, 0, sizeof(errbuff));

    //Comprobación de parámetros enviados al programa. 
    if (argc != 4) {
        fprintf(stderr, "Error: Sintaxis: /mi_cp <nombre_disco> /ruta_origen /ruta_destino/\n");
        exit(-1);
    }

    if (argv[3][strlen(argv[3]) - 1] != '/') {
        fprintf(stderr, "Error: ruta de destino ha de ser un directorio.\n");
        exit(-1);
    }

    //Montaje del disco virtual. 
    if (bmount(argv[1]) == -1) {
        fprintf(stderr, "Error: no se ha podido abrir el directorio indicado.\n");
        exit(-1);
    }

    //Llamada a la función mi_unlink. 
    if (mi_cp(argv[2], argv[3]) < 0) {
        exit(-1);
    }

    //Desmontaje del dispositivo virtual. 
    if (bumount() == -1) {
        fprintf(stderr, "Error: no se ha podido cerrar el fichero.\n");
        exit(-1);
    }
}
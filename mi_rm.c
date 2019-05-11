/*
    PROGRAMA PARA BORRAR UN FICHERO O DIRECTORIO. 
*/

#include "bloques.h"
#include "directorios.h"
#include <string.h>

int main(int argc, char **argv) {

    //Declaraciones
    int var = 0;
    char errbuff[BLOCKSIZE]; 

    //Inicialización del buffer de errores. 
    memset(errbuff, 0, sizeof(errbuff));

    //Comprobación de parámetros enviados al programa. 
    if (argc != 3) {
        fprintf(stderr, "Error: Sintaxis: /mi_rm disco /ruta\n");
        exit(-1);
    }



    //Montaje del disco virtual. 
    if (bmount(argv[1]) == -1) {
        fprintf(stderr, "Error: no se ha podido abrir el directorio indicado.\n");
        exit(-1);
    }

    



    //Desmontaje del dispositivo virtual. 
    if (bumount() == -1) {
        fprintf(stderr, "Error: no se ha podido cerrar el fichero.\n");
        exit(-1);
    }
}
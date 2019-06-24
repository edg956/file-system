/*
    PROGRAMA PARA COPIAR UN FICHERO/DIRECTORIO DENTRO DE OTRO DIRECTORIO 
*/

#include "bloques.h"
#include "directorios.h"
#include <string.h>

int main(int argc, char **argv) {

    char errbuff[BLOCKSIZE]; //Buffer de errores.
    memset(errbuff, 0, sizeof(errbuff));
    int nerror;
    struct STAT stat;

    //Comprobación de parámetros enviados al programa. 
    if (argc != 4) {
        fprintf(stderr, "Error: Sintaxis: /mi_cp <nombre_disco> /ruta_origen /ruta_destino/\n");
        exit(-1);
    }

    //Montaje del disco virtual. 
    if (bmount(argv[1]) == -1) {
        fprintf(stderr, "Error: no se ha podido abrir el directorio indicado.\n");
        exit(-1);
    }

    //Verificar que destino sea un directorio
    if (mi_stat(argv[3], &stat) == -1) {
        fprintf(stderr, "Error: no se ha podido obtener el stat de la ruta destino\n");
        exit(-1);
    }

    if (stat.tipo != T_INODO_DIRECTORIO) {
        fprintf(stderr, "Error: ruta de destino ha de ser un directorio.\n");
        exit(-1);
    }

    //Llamada a la función mi_cp. 
    if ((nerror = mi_cp_r(argv[2], argv[3])) < 0) {
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
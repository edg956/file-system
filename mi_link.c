/*
    PROGRAMA PARA ENLAZAR UNA RUTA A UN FICHERO EN OTRA RUTA
*/
#include "bloques.h"
#include "directorios.h"
#include <string.h>

int main(int argc, char **argv) {

    int var = 0;
    char errbuff[BLOCKSIZE]; 

    //Inicialización del buffer de errores. 
    memset(errbuff, 0, sizeof(errbuff));

    //Comprobación de parámetros enviados al programa. 
    if (argc != 4) {
        fprintf(stderr, "Error: formato de comando: ./nombre_de_programa <nombre_de_fichero_disco> <permisos> </ruta>\n");
        exit(-1);
    }

    //Comprobar que las rutas se correspondan a ficheros
    //Comprobación ruta origen
    while (argv[2][var] != '\0') var++;

    if (argv[2][var-1] == '/') {
        fprintf(stderr, "Error: ruta origen debe corresponder con la ruta de un"
        " fichero.\n");
        exit(-1);
    }

    //Comprobación ruta a enlazar
    var = 0;
    while (argv[3][var] != '\0') var++;

    if (argv[3][var-1] == '/') {
        fprintf(stderr, "Error: ruta a enlazar debe corresponder con la ruta de"
        " un fichero.\n");
        exit(-1);
    }

    //Montaje del disco virtual. 
    if (bmount(argv[1]) == -1) {
        fprintf(stderr, "Error: no se ha podido abrir el directorio indicado.\n");
        exit(-1);
    }

    //Llamada a mi_creat para crear directorio/fichero
    var = mi_link(argv[2], argv[3]);

    //Check errores
    if (var == -1) {
        fprintf(stderr, "Error: no se ha podido crear directorio o fichero.\n");
        exit(-1);
    }

    //Check errores de buscar entrada. 
    if (var < -1) {
        control_errores_buscar_entrada(var, errbuff);
        fprintf(stderr, "%s", errbuff);
    }

    //Desmontaje del dispositivo virtual. 
    if (bumount() == -1) {
        fprintf(stderr, "Error: no se ha podido cerrar el fichero.\n");
        exit(-1);
    }
}
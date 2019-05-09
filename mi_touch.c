/*
    PROGRAMA PARA CREAR NUEVOS FICHEROS.
*/
#include "bloques.h"
#include "directorios.h"
#include <string.h>

int main(int argc, char **argv) {

    int result = 0;

    //Comprobación de parámetros enviados al programa. 
    if (argc != 4) {
        fprintf(stderr, "Error: formato de comando: ./nombre_de_programa <nombre_de_fichero_disco> <permisos> </ruta>\n");
        exit(-1);
    }

    //Comprobar correción de permisos introducidos
    if (strlen(argv[2]) != 1 || (atoi(argv[2]) & 248) != 0) {
        fprintf(stderr, "Error: rango permitido para permisos: [0,7]");
        exit(-1);
    }

    //Montaje del disco virtual. 
    if (bmount(argv[1]) == -1) {
        fprintf(stderr, "Error: no se ha podido abrir el directorio indicado.\n");
        exit(-1);
    }

    //Llamada a mi_creat para crear fichero
    result = mi_touch(argv[3], atoi(argv[2]));

    //Check errores
    if (result == -1) {
        //fprintf(stderr, "Error: no se ha podido crear directorio o fichero.\n");
        exit(-1);
    }

    //Desmontaje del dispositivo virtual. 
    if (bumount() == -1) {
        fprintf(stderr, "Error: no se ha podido cerrar el fichero.\n");
        exit(-1);
    }
}
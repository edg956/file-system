/*
    PROGRAMA PARA CREAR NUEVOS FICHEROS.
*/
#include "bloques.h"
#include "directorios.h"
#include <string.h>

int main(int argc, char **argv) {

    int result = 0;
    char errbuff[BLOCKSIZE]; //Buffer de errores. 

    //Comprobación de parámetros enviados al programa. 
    if (argc != 4) {
        fprintf(stderr, "Error: formato de comando: ./nombre_de_programa <nombre_de_fichero_disco> <permisos> </ruta>\n");
        exit(-1);
    }

    //Comprobar correción de permisos introducidos
    if (strlen(argv[2]) != 1 || (atoi(argv[2]) & 248) != 0) {
        fprintf(stderr, "Error: Rango de permisos incorrecto. Ej: [0,7]\n");
        exit(-1);
    }

    //Montaje del disco virtual. 
    if (bmount(argv[1]) == -1) {
        fprintf(stderr, "Error: no se ha podido abrir el directorio indicado.\n");
        exit(-1);
    }

    //Llamada a mi_creat para crear fichero
    result = mi_touch(argv[3], atoi(argv[2]));

    //Inicialización del buffer de errores. 
    memset(errbuff, 0, sizeof(errbuff));

    //Check errores
    if (result < 0) {
        control_errores_buscar_entrada(result, errbuff);
        fprintf(stderr, "%s", errbuff);
        exit(-1);
    }

    //Desmontaje del dispositivo virtual. 
    if (bumount() == -1) {
        fprintf(stderr, "Error: no se ha podido cerrar el fichero.\n");
        exit(-1);
    }
}
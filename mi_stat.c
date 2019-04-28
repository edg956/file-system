/*
    PROGRAMA PARA CONSULTAR LOS METADATOS DE DIRECTORIOS O FICHEROS
*/
#include "bloques.h"
#include "directorios.h"

int main(int argc, char **argv) {

    //Comprobación de parámetros enviados al programa. 
    if (argc != 3) {
        perror("Error: formato de comando: ./nombre_de_programa "
        "<nombre_de_fichero_disco> </ruta>\n");
        exit(-1);
    }

    //Montaje del disco virtual. 
    if (bmount(argv[1]) == -1) {
        perror("Error: no se ha podido abrir el directorio indicado.\n");
        exit(-1);
    }


    //Desmontaje del dispositivo virtual. 
    if (bumount() == -1) {
        perror("Error: no se ha podido cerrar el fichero.\n");
        exit(-1);
    }
}
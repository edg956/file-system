/*
    PROGRAMA PARA MODIFICAR LOS PERMISOS DE LECTURA, ESCRITURA Y EJECUCIÓN A
    DIRECTORIOS O FICHEROS
*/
#include "bloques.h"
#include "directorios.h"

int main(int argc, char **argv) {

    //Comprobación de parámetros enviados al programa. 
    if (argc != 4) {
        perror("Error: formato de comando: ./nombre_de_programa <nombre_de_fichero_disco> <permisos> </ruta>\n");
        exit(-1);
    }

    //Comprobar correción de permisos introducidos
    if (strlen(argv[2]) != 1 || (atoi(argv[2]) & 248) != 0) {
        perror("Error: rango permitido para permisos: [0,7]");
        exit(-1);
    }

    //Montaje del disco virtual. 
    if (bmount(argv[1]) == -1) {
        perror("Error: no se ha podido abrir el directorio indicado.\n");
        exit(-1);
    }

    //Llamada a mi_chmod()
    if (mi_chmod(argv[3], (unsigned char) argv[2][0]) == -1) {
        perror("Error: no se ha podido modificar permisos.");
        exit(-1);
    }

    //Desmontaje del dispositivo virtual. 
    if (bumount() == -1) {
        perror("Error: no se ha podido cerrar el fichero.\n");
        exit(-1);
    }
}
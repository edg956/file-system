/*
    PROGRAMA PARA CREAR NUEVOS DIRECTORIOS O FICHEROS
*/
#include "bloques.h"
#include "directorios.h"
<<<<<<< HEAD
#include <string.h>

int main(int argc, char **argv) {

    int result = 0;

=======

int main(int argc, char **argv) {

>>>>>>> Implementadas funciones casi totalmente. Creados ficheros de programas de nivel 9 y agregados al makefile.
    //Comprobación de parámetros enviados al programa. 
    if (argc != 4) {
        perror("Error: formato de comando: ./nombre_de_programa <nombre_de_fichero_disco> <permisos> </ruta>\n");
        exit(-1);
    }

<<<<<<< HEAD
    //Comprobar correción de permisos introducidos
    if (strlen(argv[2]) != 1 || (atoi(argv[2]) & 248) != 0) {
        perror("Error: rango permitido para permisos: [0,7]");
        exit(-1);
    }

=======
>>>>>>> Implementadas funciones casi totalmente. Creados ficheros de programas de nivel 9 y agregados al makefile.
    //Montaje del disco virtual. 
    if (bmount(argv[1]) == -1) {
        perror("Error: no se ha podido abrir el directorio indicado.\n");
        exit(-1);
    }

<<<<<<< HEAD
    //Llamada a mi_creat para crear directorio/fichero
    result = mi_creat(argv[3], argv[2][0]);

    //Check errores
    if (result == -1) {
        perror("Error: no se ha podido crear directorio o fichero.");
        exit(-1);
    }
=======
>>>>>>> Implementadas funciones casi totalmente. Creados ficheros de programas de nivel 9 y agregados al makefile.

    //Desmontaje del dispositivo virtual. 
    if (bumount() == -1) {
        perror("Error: no se ha podido cerrar el fichero.\n");
        exit(-1);
    }
}
#include "directorios.h"

/*
Permite escribir texto en una posición de un fichero (offset).
    - Ha de recibir comoo parámetro la ruta del fichero en vez del ninodo.

    - Eliminar el argumento de diferentes_inodos y añadir el de offset para 
    poder indicar desde consola que vamos a escribir.

Hay que comprobar que se trate de un fichero, y que tenga permisos de escritura.
Ha de mostrar la cantidad de bytes escritos.
*/
int main (int argc, char **argv) {

    //Declaraciones.
    unsigned int longitud = 0;
    unsigned int offset = 0;
    int bytesEscritos = 0;
    char *camino = argv[2];

    //Comprobación de los argumentos pasados por parámetro. 
    if(argc != 5) {
        fprintf(stderr, "Error de sintaxis: mi_escribir <disco> </ruta_fichero> <texto> <offset>\n");
        exit(-1);
    }

    //Comprobar que el camino es un fichero (no se puede escribir en directorios).
    if(camino[strlen(camino) - 1] == '/'){
        fprintf(stderr, "Error: La ruta no es un fichero.\n");
        exit(-1);
    }

    //Montaje del disco virtual. 
    if (bmount(argv[1]) == -1) {
        fprintf(stderr, "Error: no se ha podido abrir el directorio indicado.\n");
        exit(-1);
    }

    //Obtención de los parámetros. 
    offset = strtol(argv[4], NULL, 10);
    longitud = strlen(argv[3]);
    char buffer[longitud];
    strcpy(buffer, argv[3]);

    printf("Longitud texto: %i\n", longitud);

    bytesEscritos = mi_write(camino, buffer, offset, longitud);

    //Comprobación de errores.
    if(bytesEscritos == -1){
        printf("Bytes escritos: %i\n", bytesEscritos+1);
        exit(-1);
    }

    printf("Bytes escritos: %i\n", bytesEscritos);

    //Desmontaje del dispositivo virtual. 
    if (bumount() == -1) {
        fprintf(stderr, "Error: no se ha podido cerrar el fichero.\n");
        exit(-1);
    }
}
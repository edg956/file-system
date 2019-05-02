#include "bloques.h"
#include "ficheros_basico.h"
#include <stdio.h>

int main(int argc, char **argv) {

    if (argc != 2) {
        perror("Error: formato de comando: ./nombre_de_programa <nombre_de_fichero_disco>\n");
        exit(-1);
    }

    if (bmount(argv[1]) == -1) {
        perror("Error: no se ha podido abrir el directorio indicado.\n");
        exit(-1);
    }

    struct inodo inodo;

    for (int i = 0; i < 5; i++) {
        leer_inodo(i, &inodo);
    
    printf("inodo: %i\n", i);
    printf("tipo: %c\n",inodo.tipo);
    printf("Permisos: %i\n",inodo.permisos);
    printf("nlinks: %i\n",inodo.nlinks);
    printf("tamEnBytesLog: %i\n",inodo.tamEnBytesLog);
    printf("numBloquesOcupados: %i\n",inodo.numBloquesOcupados);
    puts("\n");
    }

    if (bumount() == -1) {
        perror("Error bumount");
        exit(-1);
    }

}
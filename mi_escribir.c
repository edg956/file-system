#include "directorios.h"


/*
Permite escribir texto en una posición de un fichero (offset).
    - Ha de recibir comoo parámetro la ruta del fichero en vez del ninodo.
    - Eliminar el argumento de diferentes_inodos y añadir el de offset para poder
    indicar desde consola que vamos a escribir.
Hay que comprobar que se trate de un fichero, y que tenga permisos de escritura.
Ha de mostrar la cantidad de bytes escritos.
*/
int main (int argc, char **argv) {

int longitud = 0;
int offset = 0;
int bytesEscritos = 0;

if(argc != 5){
    perror("Error de sintaxis: mi_escribir <disco> </ruta_fichero> <texto> <offset>\n");
    exit(-1);
}
char *camino = argv[2];

if(camino[strlen(camino) - 1] == '/'){
    perror("Error: La ruta no es un fichero.\n");
    exit(-1);
}

offset = atoi(argv[4]); // no se si esta bien pero sino no sabia como hacerlo
longitud = strlen(argv[3]);
char buffer[longitud];
strcpy(buffer, argv[3]);

bytesEscritos = mi_write(camino, &buffer, offset, longitud);
if(bytesEscritos == -1){
    perror ("Error.\n");
    exit(-1);
}

printf("Longitud texto: %i\n", longitud);
printf("Bytes escritos: %i\n", bytesEscritos);

}
/*
    Programa que creará el dispositivo virtual con el tamaño adecuado.
    Debe ser llamado desde la linea de comandos con los siguientes parámetros
    para dar nombre al dispositivo virtual y determinar la cantidad de bloques
    de que dispondrá nuestro sistema de ficheros:

    $ ./mi_mkfs <nombre_dispositivo> <nbloques>
*/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <bloques.h>

#define INITIALIZATION_VALUE 0


int main(int argc, char **argv) {

    //Comprobar si se ha realizado la llamada correctamente. 
    if ((argv[1]==NULL)||(argv[2]==NULL)) {

        printf("ERROR: La ejecución del programa no se ha realizado correctamente.\n");
        printf("Requisitos para la ejecución del programa: \n");
        printf("Primer parámetro -> nombre_del_dispositivo.\n");
        printf("Segundo parámetro -> número_de_bloques.\n"); 
        printf("Ejemplo: \n");
        printf("./mi_mkfs nombre_del_dispositivo número_de_bloques\n");
        exit(-1);
    }

    if (bmount(argv[1])==-1) {
        printf("Se ha producido un error al montar el dispositivo virtual.");
        exit(-1);
    }

    for(int i = 0; i < argv[2]; i++) {
        
        unsigned char buffer[BLOCKSIZE];
        memset(buffer, INITIALIZATION_VALUE, BLOCKSIZE);

        bwrite(i, buffer);

    }

    bumount();

}
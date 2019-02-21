/*
    Programa que creará el dispositivo virtual con el tamaño adecuado.
    Debe ser llamado desde la linea de comandos con los siguientes parámetros
    para dar nombre al dispositivo virtual y determinar la cantidad de bloques
    de que dispondrá nuestro sistema de ficheros:

    $ ./mi_mkfs <nombre_dispositivo> <nbloques>
*/

//Includes. 
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "bloques.h"

//Defines.
#define INITIALIZATION_VALUE 0

/* Método principal.
    Descripción: 
        Se encarga de comprobar que se ejecuta el programa correctamente
        con los parámetros adecuados. 
        Monta y crea el disco virtual. 

    Funciones a las que llama: 
        + bloques.c - bmount()
        + bloques.c - bwrite()
        + bloques.c - bumount()
        
    Parámetros de entrada: 
        + argc: número de elementos de entrada. 
        + **argv: array de los elementos de entrada. 

    Parámetros de salida: 
        + 0: Si se ha ejecutado sin errores. 
        + (-1): Si ha habido algún error.
*/
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

    //Recoger los valores pasados por parámetros. 
    char *file_name = argv[1];
    int number_of_blocks = atoi(argv[2]);

    //Realizar el montaje del disco virtual.
    if (bmount(file_name)==-1) {
        printf("Se ha producido un error al montar el dispositivo virtual.");
        exit(-1);
    }
    
    //Creación del buffer e inicialización a 0.
    unsigned char buffer[BLOCKSIZE];
    memset(buffer, INITIALIZATION_VALUE, BLOCKSIZE);

    //Inicializar el dispositivo virtual. 
    for(int i = 0; i <number_of_blocks; i++) {
        bwrite(i, buffer);
    }

    //Desmontar el dispositivo virtual. 
    if (bumount()==-1) {
        printf("Se ha producido un error al desmontar el dispositivo virtual.");
        exit(-1);
    }
    return 0;
}
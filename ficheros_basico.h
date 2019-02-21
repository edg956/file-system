#include <stdio.h>  //printf(), fprintf(), stderr, stdout, stdin
#include <fcntl.h> //O_WRONLY, O_CREAT, O_TRUNC
#include <sys/stat.h> //S_IRUSR, S_IWUSR
#include <stdlib.h>  //exit(), EXIT_SUCCESS, EXIT_FAILURE, atoi()
#include <unistd.h> // SEEK_SET, read(), write(), open(), close(), lseek()
#include <errno.h>  //errno
#include <string.h> // strerror()
#include "bloques.h"

#define BLOCKSIZE 1024 // bytes
#define INODOSIZE 128  // bytes
#define posSB 0        //el superbloque se escribe en el primer bloque de nuestro FS

struct superbloque{
	unsigned int posPrimerBloqueMB; //Posición del primer bloque del mapa de bits 
	unsigned int posUltimoBloqueMB; //Posición del último bloque del mapa de bits 
	unsigned int posPrimerBloqueAI; //Posición del primer bloque del array de inodos 
	unsigned int posUltimoBloqueAI; //Posición del último bloque del array de inodos 
	unsigned int posPrimerBloqueDatos; //Posición del primer bloque de datos 
	unsigned int posUltimoBloqueDatos; //Posición del último bloque de datos 
	unsigned int posInodoRaiz; //Posición del inodo del directorio raíz 
	unsigned int posPrimerInodoLibre; //Posición del primer inodo libre 
	unsigned int cantBloquesLibres; //Cantidad de bloques libres
	unsigned int cantInodosLibres; //Cantidad de inodos libres 
	unsigned int totBloques; //Cantidad total de bloques 
	unsigned int totInodos; //Cantidad total de inodos 
	char padding[BLOCKSIZE-12*sizeof(unsigned int)]; //Relleno
};
//aqui plis

/*
    Definición de estructura inodo
*/
//aquí plis

int tamMB(unsigned int nbloques);
int tamAI(unsigned int ninodos);
int initSB(unsigned int nbloques, unsigned int ninodos);
int initMB();
int initAI();
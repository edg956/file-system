#include <stdio.h>  //printf(), fprintf(), stderr, stdout, stdin
#include <fcntl.h> //O_WRONLY, O_CREAT, O_TRUNC
#include <sys/stat.h> //S_IRUSR, S_IWUSR
#include <stdlib.h>  //exit(), EXIT_SUCCESS, EXIT_FAILURE, atoi()
#include <unistd.h> // SEEK_SET, read(), write(), open(), close(), lseek()
#include <errno.h>  //errno
#include <string.h> // strerror()
#include "bloques.h"
#include <sys/types.h>

#include <time.h>

#define BLOCKSIZE 1024 // bytes
#define INODOSIZE 128  // bytes
#define NUMINPRBLQ (BLOCKSIZE / INODOSIZE)    //Nº de inodos por bloque
#define posSB 0 //el superbloque se escribe en el primer bloque de nuestro FS

#define NPUNTEROS (BLOCKSIZE / sizeof(unsigned int))
#define DIRECTOS 12
#define INDIRECTOS0 (NPUNTEROS + DIRECTOS)
#define INDIRECTOS1 (NPUNTEROS * NPUNTEROS + INDIRECTOS0)
#define INDIRECTOS2 (NPUNTEROS * NPUNTEROS * NPUNTEROS + INDIRECTOS1)

#define T_INODO_LIBRE 'l'
#define T_INODO_DIRECTORIO 'd'
#define T_INODO_FICHERO 'f'

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

/*
    Definición de estructura inodo.
*/
struct inodo { 
   char tipo;     // Tipo ('l':libre, 'd':directorio o 'f':fichero)
   char permisos; // Permisos (lectura y/o escritura y/o ejecución)
   char reservado_alineacion1[6];
   time_t atime; // Fecha y hora del último acceso a datos: atime
   time_t mtime; // Fecha y hora de la última modificación de datos: mtime
   time_t ctime; // Fecha y hora de la última modificación del inodo: ctime
   unsigned int nlinks;             // Cantidad de enlaces de entradas en directorio
   unsigned int tamEnBytesLog;      // Tamaño en bytes lógicos
   unsigned int numBloquesOcupados; // Cantidad de bloques ocupados zona de datos
   unsigned int punterosDirectos[12];  // 12 punteros a bloques directos
   unsigned int punterosIndirectos[3]; // 3 punteros a bloques indirectos
   char padding[INODOSIZE - 2 * sizeof(unsigned char) - 3 * sizeof(time_t) - 18 * sizeof(unsigned int) - 6 * sizeof(unsigned char)];
};

/*FUNCIONES NIVEL 2*/
int tamMB(unsigned int nbloques);
int tamAI(unsigned int ninodos);
int initSB(unsigned int nbloques, unsigned int ninodos);
int initMB();
int initAI();

/*FUNCIONES NIVEL 3*/
int escribir_bit(unsigned int nbloque, unsigned int bit);
unsigned char leer_bit(unsigned int nbloque);
int reservar_bloque();
int liberar_bloque(unsigned int nbloque);
int escribir_inodo(unsigned int ninodo, struct inodo inodo);
int leer_inodo(unsigned int ninodo, struct inodo *inodo);
int reservar_inodo(unsigned char tipo, unsigned char permisos);

/*FUNCIONES NIVEL 4*/
int obtener_nrangoBL(struct inodo inodo, unsigned int nblogico, unsigned int *ptr);
int obtener_indice(int nblogico, int nivel_punteros); 
int traducir_bloque_inodo(unsigned int ninodo, unsigned int nblogico, char reservar);

/*FUNCIONES NIVEL 5*/
int liberar_inodo(unsigned int ninodo);
int liberar_bloques_inodo(unsigned int ninodo, unsigned int nblogico);

#include <stdio.h> 
#include "ficheros.h"
#include "ficheros_basico.h"

/*FUNCIONES NIVEL 8*/
int extraer_camino(const char *camino, char *inicial, char *final, char *tipo);
int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo_dir, unsigned int *p_inodo, unsigned int *p_entrada, char reservar, unsigned char permisos);

/*FUNCIONES NIVEL 9*/
int mi_creat(const char *camino, unsigned char permisos);
int mi_dir(const char *camino, char *buffer);
int mi_chmod(const char *camino, unsigned char permisos);
int mi_stat(const char *camino, struct STAT *p_stat);

//Estructura entrada. 
struct entrada {
    char nombre[60];  //En el SF ext2 la longitud del nombre es 255
    unsigned int ninodo;
};
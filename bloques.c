#include "bloques.h"

/*
    Función que llama a la función open para obtener el descriptor de fichero de
    obtener el descriptor de fichero que se utilizará como dispositivo virtual.
    
    Parámetros:
        +camino: String que indica el path del fichero a abrir
    Devuelve:
        +Descriptor de fichero solicitado.
        +(-1): En caso de error.
*/
int bmount(const char *camino) {}

/*
    Función que llama a la función close con el descriptor de fichero obtenido
    por bmount() para liberarlo.
    
    Devuelve:
        +0: En funcionamiento correcto.
        +(-1): En caso de error.
*/
int bumount() {}

/*
    Escribe el contenido de un buffer de memoria en el bloque del dispositivo
    virtual especificado. Realiza llamadas a las funciones write() y lseek()

    Parámetros:
        +nbloque: El número de bloque donde volcar el contenido del buffer.
        +buf: Puntero al buffer de memoria a volcar.
    Devuelve:
        +El número de bytes que ha escrito.
        +(-1): En caso de error.
*/
int bwrite(unsigned int nbloque, const void *buf) {}

/*
    Lee desde el dispositivo virtual el bloque de memoria especificado y
    copia su contenido en un buffer. Realiza llamadas a las funciones read()
    y lseek()

    Parámetros:
        +nbloque: El número de bloque desde el cual leer información.
        +buf: Puntero al buffer en memoria donde copiar los datos.
    Devuelve:
        +El número de bytes que ha leído.
        +(-1): En caso de error.
*/
int bread(unsigned int nbloque, void *buf) {}

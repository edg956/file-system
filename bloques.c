#include "bloques.h"

//Variables globales.
static int descriptor = 0;
static sem_t *mutex;
static unsigned int inside_sc = 0;

/*
    Descripción: 
        Función que llama a la función open para obtener el descriptor de fichero 
        que se utilizará como dispositivo virtual.
    
    Funciones desde dónde es llamado: 
        + mi_mkfs.c - main()
        + leer_sf.c - main()

    Parámetros de entrada:
        + camino: String que indica el path del fichero a abrir
    
    Parámetros de salida: 
        + Descriptor de fichero solicitado.
        + (-1): En caso de error.
*/
int bmount(const char *camino) {

    if (descriptor > 0) {
       close(descriptor);
    }
    
    if ((descriptor = open(camino, O_RDWR | O_CREAT, 0666)) == -1) {
       fprintf(stderr, "Error: bloques.c → bmount() → open()\n");
    }
    
    if (!mutex) { //el semáforo es único y sólo se ha de inicializar una vez en nuestro sistema
        mutex = initSem();
        if (mutex == SEM_FAILED) {
           return -1;
        }
    }
    return descriptor;
}

/*
    Descripción: 
        Función que llama a la función close con el descriptor de fichero obtenido
        por bmount() para liberarlo.

    Funciones desde dónde es llamado: 
        + mi_mkfs.c - main()
        + leer_sf.c - main()
    
    Parámetros de salida:
        + 0: En funcionamiento correcto.
        + (-1): En caso de error.
*/
int bumount() {

 descriptor = close(descriptor); 
   // hay que asignar el resultado de la operación a la variable ya que bmount() la utiliza
   if (descriptor == -1) {
       fprintf(stderr, "Error: bloques.c → bumount() → close(): %d: %s\n", errno, strerror(errno));
       return -1;
   }
   deleteSem(); // borramos semaforo
   return 0;
   
}

/*
    Descripción: 
        Escribe el contenido de un buffer de memoria en el bloque del dispositivo
        virtual especificado. Realiza llamadas a las funciones write() y lseek()
        
    Funciones desde dónde es llamado: 
        + mi_mkfs.c - main()
        + ficheros_basico.h - initSB()
        + ficheros_basico.h - initMB()
        + ficheros_basico.h - initAI()
        + ficheros_basico.h - escribir_bit()
        + ficheros_basico.h - reservar_bloque()
        + ficheros_basico.h - liberar_bloque()
        + ficheros_basico.h - escribir_inodo()
        + ficheros_basico.h - reservar_inodo()
        + ficheros_basico.h - traducir_bloques_inodo()
        + ficheros_basico.h - liberar_inodo()
        + ficheros_basico.h - liberar_bloques_inodo()
        + ficheros.h - mi_write_f()        

    Parámetros de entrada:
        + nbloque: El número de bloque donde volcar el contenido del buffer.
        + *buf: Puntero al buffer de memoria a volcar.

    Parámetros de salida:
        + El número de bytes que ha escrito.
        + (-1): En caso de error.
*/
int bwrite(unsigned int nbloque, const void *buf) {

    int bwritten;

    //Mover puntero de escritura/lectura hasta el bloque deseado
    if (lseek(descriptor, nbloque*BLOCKSIZE, SEEK_SET) != nbloque*BLOCKSIZE) {
        return -1;  //Check for errors
    }

    //Escribir contenido del buffer en el bloque indicado
    bwritten = write(descriptor, buf, BLOCKSIZE);
    if (bwritten != BLOCKSIZE) {
        return -1;  //Check for errors
    }

    return bwritten;
}

/*  Descripción: 
        Lee desde el dispositivo virtual el bloque de memoria especificado y
        copia su contenido en un buffer. Realiza llamadas a las funciones read()
        y lseek()
    
    Funciones desde dónde es llamado: 
        + mi_mkfs.c - main()
        + ficheros_basico.h - initMB()
        + ficheros_basico.h - initAI()
        + ficheros_basico.h - escribir_bit()
        + ficheros_basico.h - leer_bit()
        + ficheros_basico.h - reservar_bloque()
        + ficheros_basico.h - liberar_bloque()
        + ficheros_basico.h - escribir_inodo()
        + ficheros_basico.h - leer_inodo()
        + ficheros_basico.h - reservar_inodo()
        + ficheros_basico.h - traducir_bloques_inodo()
        + ficheros_basico.h - liberar_inodo()
        + ficheros_basico.h - liberar_bloques_inodo()
        + ficheros.h - mi_write_f()        
        + ficheros.h - mi_read_f()        

    Parámetros de entrada:
        + nbloque: El número de bloque desde el cual leer información.
        + buf: Puntero al buffer en memoria donde copiar los datos.
    
    Parámetros de salida: 
        + El número de bytes que ha leído.
        + (-1): En caso de error.
*/
int bread(unsigned int nbloque, void *buf) {
    int bbread;

    //Mover puntero de escritura/lectura hasta el bloque deseado
    if (lseek(descriptor,nbloque*BLOCKSIZE,SEEK_SET) != nbloque*BLOCKSIZE) {
        return -1;
    }

    bbread = (int)read(descriptor, buf, BLOCKSIZE);
    if (bbread == -1){
        return -1;
    }

    return bbread;
}

/*  Descripción: 
        Método que ejecuta la función lock de un mutex
    
    Funciones desde dónde es llamado:         
*/
void mi_waitSem() {
    if (!inside_sc) {
        waitSem(mutex);
    }
    inside_sc++;
}

/*  Descripción: 
        Método que ejecuta la función unlock de un mutex
    
    Funciones desde dónde es llamado:         
*/
void mi_signalSem() {
    inside_sc--;
    if (!inside_sc) {
        signalSem(mutex);
    }
}
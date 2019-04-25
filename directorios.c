#include <string.h>
#include "directorios.h"
#define BUF_SIZE BLOCKSIZE

/*
    Descripción:
        Dada una cadena de caracteres camino (que comience por '/'), 
        separa su contenido en dos.
        
        Ejemplos:
            camino = "/dir1/dir2/fichero"
            inicial = "dir1" (devuelve DIRECTORIO)
            final = "/dir2/fichero"

    Funciones a las que llama:

        
    Funciones desde donde es llamado:


    Parámetros de entrada:
        + const char *camino
        + char *inicial
        + char *final
        + char *tipo 

    Parámetros de salida:
*/
int extraer_camino(const char *camino, char *inicial, char *final, char *tipo) {
    if (camino[0] != '/'){
        return -1;              //Devuelve -1 en caso de que el camino no comience por el carácter '/'
    }
    int i = 1;
    /*
    En el siguiente bucle guardaremos en inicial la porción comprendida entre los dos primeros '/' o en el caso
    que no haya un segundo '/' guarda camino en inicial también.
    */
    while ((camino[i] != '/') && (camino[i] != '\0')){  
        inicial[i-1] = camino[i];
        i++;
    }
    
    if (i == strlen(camino)){
        final = "\0";
        tipo = "f";
        return 0;               //Devolvemos 0 por que lo que hay en el camino corresponde a un fichero
    }

    int j = 0;
    while (i < strlen(camino)){
        final[j] = camino[i];
        i++;
        j++;
    }
    tipo = "d";
    return 1;                   //Devolvemos 1 por quelo que hay en el camino corresponde a un directorio
}


/*
    Descripción:    
        Como su nombre indica, esta función nos buscará la entrada indicada, 
        entre las entradas del inodo correspondiente a su directorio padre.

    Funciones a las que llama:
        + Extraer camino
        
    Funciones desde donde es llamado:

    Parámetros de entrada:
        + const char *camino_parcial
        + unsigned int *p_inodo_dir
        + unsigned int *p_inodo
        + unsigned int *p_entrada
        + char reservar
        + unsigned char permisos

    Parámetros de salida:

*/
int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo_dir, unsigned int *p_inodo, unsigned int *p_entrada, char reservar, unsigned char permisos) {

    //Declaraciones
    char *inicial; 
    char *final; 
    char *tipo; 

    //Caso en el que sólo se pasa la raíz. 
    if (strcmp(camino_parcial, "/")) {
        *p_inodo = 0; 
        *p_entrada = 0;
        return 0; 
    }

    if (extraer_camino(camino_parcial, inicial, final, &tipo)==-1) {
        fprintf(stderr, "Error al extraer camino. Función -> buscar_entrada()");
        exit(-1);
    }

    //Buscamos la entrada cuyo nombre se encuentra en la inicial. 
    leer_inodo(*p_inodo_dir, &p_inodo_dir);
    
    if ((permisos & 4)!=4) {
        fprintf(stderr, "Error en los permisos de lectura del inodo."
        "Función -> buscar_entrada()");
    } 

    //Declaraciones.
    /*Inicializar el buffer de lectura para llamar a mi_read_f()
    Se utilizará un struct entrada*/
    struct entrada entrada; 
    int numentradas = 0; 
    int nentrada = 0;

    //Calcular el número de entradas del inodo (numentradas).
    /*Nunca se cuenta la última barra "/" (-1)*/
    for(int i = 0; i<strlen(camino_parcial)-1; i++) {
        if (strcmp(camino_parcial[i], "/")) {
            numentradas++;
        }
    }

     if (numentradas > 0) {
         mi_read_f(p_inodo_dir, entrada.nombre, offset, nbytes); //--?

        while ((nentrada < numentradas)&&(inicial != entrada.nombre)) {
            nentrada++; 
            mi_read_f();
        }

     }

     if (nentrada==numentradas) {

         seleccionar reservar; 

         switch (reservar) {
             //Modo consulta. Como no existe retornamos error. 
             case 0: 
                return no existe entrada; 
                break; 

                //Modo escritura. 
                case 1: 

         }
     }
}
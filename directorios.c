#include <string.h>
#include "directorios.h"
#define BUF_SIZE BLOCKSIZE
#define NAME_MAX_LENGTH 60     //EXT2 filename size limit

/*----------------------------FUNCIONES DE NIVEL 8----------------------------*/

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

    inicial[i-1] = "\0"; //Marcar fin de String.
    
    if (i == strlen(camino)){
        *final = "\0";
        *tipo = "f";
        return 0;               //Devolvemos 0 por que lo que hay en el camino corresponde a un fichero
    }

    int j = 0;
    while (i < strlen(camino)){
        final[j] = camino[i];
        i++;
        j++;
    }
    *tipo = "d";
    final[j] = "\0";
    return 1;                   //Devolvemos 1 por quelo que hay en el camino corresponde a un directorio
}


/*
    Descripción:    
        Como su nombre indica, esta función nos buscará la entrada indicada, 
        entre las entradas del inodo correspondiente a su directorio padre.

    Funciones a las que llama:
        +
        + directorios.h - extraer_camino()
        + directorios.h - buscar_entrada()
        
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
    char inicial[NAME_MAX_LENGTH]; 
    char final[NAME_MAX_LENGTH]; 
    char tipo; 
    struct inodo inodo_dir;

    //Caso en el que sólo se pasa la raíz. 
    if (strcmp(camino_parcial, "/")) {
        *p_inodo = 0; 
        *p_entrada = 0;
        return 0; 
    }

    //inicializar variables
    memset(inicial, 0, NAME_MAX_LENGTH);
    memset(final, 0, NAME_MAX_LENGTH);

    if (extraer_camino(camino_parcial, inicial, final, &tipo)==-1) {
        fprintf(stderr, "Error al extraer camino. Función -> buscar_entrada()");
        exit(-1);   //exit -1?
    }

    //Buscamos la entrada cuyo nombre se encuentra en la inicial. 
    if (leer_inodo(*p_inodo_dir, &inodo_dir) == -1) {
        fprintf(stderr, "Error en lectura del inodo."
        "Función -> buscar_entrada()\n");
        return -1;
    }
    
    if ((inodo_dir.permisos & 4)!=4) {
        fprintf(stderr, "Error en los permisos de lectura del inodo."
        "Función -> buscar_entrada()");
        //No return?
    } 

    //Declaraciones.
    /*Inicializar el buffer de lectura para llamar a mi_read_f()
    Se utilizará un struct entrada*/
    //struct entrada entrada;
    struct entrada entradas[BLOCKSIZE/sizeof(struct entrada)];
    int numentradas = 0; 
    int nentrada = 0;
    int offset = 0;             //Offset dentro del inodo donde se lee/escribe
    int index = 0;              //Indice dentro del buffer de entradas

    //Poner buffers a 0's
    //memset(entrada.nombre, 0, sizeof(entrada.nombre));
    memset(entradas, 0, BLOCKSIZE);

    //Calcular el número de entradas del inodo (numentradas).
    /*Nunca se cuenta la última barra "/" (-1)*/
    for(int i = 0; i<strlen(camino_parcial)-1; i++) {
        if (strcmp(camino_parcial[i], "/")) {
            numentradas++;
        }
    }

    //Utilizar buffer de tamaño blocksize para las entradas y reducir accesos
    if (numentradas > 0) {

        //Leer un blocksize de entradas
        mi_read_f(p_inodo_dir, entradas, offset, BLOCKSIZE);
        
        while ((nentrada < numentradas) &&
               (strcmp(inicial, entradas[index].nombre) != 0)) {
            nentrada++;
            index++;
            offset += sizeof(struct entrada);   //Guardar offset para posterior escritura

            if (index == BLOCKSIZE/sizeof(struct entrada)) {
                index = 0;
                //Leer siguiente bloque de entradas
                mi_read_f(p_inodo_dir, entradas, offset, BLOCKSIZE);
            }
        }
    }

    if (nentrada==numentradas) { //Se ha recorrido el total de entradas
        index = 0;
        switch (reservar) {

            //Modo consulta. Como no existe retornamos error. 
            case 0: 

                fprintf(stderr, "Error: No existe entrada consulta. "
                    "Función -> buscar_entrada()");
                return -1; 
                break; 

            //Modo escritura. 
            case 1: 

                if (inodo_dir.tipo = 'f') {
                    fprintf(stderr, "Error no se puede crear entrada en un fichero."
                    "Función -> buscar_entrada()");
                    return -1;
                }

                //Si es directorio comprobar que tiene permiso de escritura. 
                if ((inodo_dir.permisos & 2)!=2) {

                    fprintf(stderr, "Error en los permisos de escritura del inodo. "
                    "Función -> buscar_entrada()");
                    return -1;
                    
                }else{

                    //Copiar inicial en el nombre de la entrada. 
                    strcpy(entradas[index].nombre, inicial);
                    
                    if (tipo == 'd') {
                        if(!strcmp(final, "/")) { //resultado esperado == 0
                            int ninodo = reservar_inodo(tipo, permisos);
                            entradas[index].ninodo = ninodo; 
                        }else{
                            //Cuelgan más directorios o ficheros. 
                             fprintf(stderr, "Error no existe directorio intermedio. "
                             "Función -> buscar_entrada()");
                        }

                    }else{
                        //Es un fichero
                        int ninodo = reservar_inodo('f', permisos); 
                        entradas[index].ninodo = ninodo; 
                    }

                    if (mi_write_f(p_inodo_dir, &entradas[index], offset, sizeof(struct entrada))==-1) {
                        
                        if (entradas[index].ninodo!=-1) {
                            liberar_inodo(entradas[index].ninodo);
                        }
                        fprintf(stderr, "EXIT_FAILURE. "
                        "Función -> buscar_entrada()");
                        exit(-1);
                }
            }
        }
    }

    if (!strcmp(final, "\0")) {
        if ((nentrada < numentradas) && (reservar=1)) {
            //Modo escritura y la entrada ya existe.
            fprintf(stderr, "Error: entrada ya existente. "
            "Función -> buscar_entrada()");
            return -1; 
        }

        //Se corta la recursividad.
        *p_inodo = entradas[index].ninodo; 
        *p_entrada = nentrada; 
        return 0; 
    }else{

        *p_inodo_dir = entradas[index].ninodo; 
        return buscar_entrada(final, p_inodo_dir, p_inodo, p_entrada, reservar, permisos);
    }
}

int mi_creat(const char *camino, unsigned char permisos){

}

int mi_dir(const char *camino, char *buffer){

}

int mi_chmod(const char *camino, unsigned char permisos){

}

int mi_stat(const char *camino, struct STAT *p_stat){
    
}
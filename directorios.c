#include <string.h>
#include "directorios.h"
#include "bloques.h"

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

/*
    Descripción:    
        Función que crea un directorio/fichero y su entrada de directorio

    Funciones a las que llama:
        + bloques.h - bread()
        + directorios.h - buscar_entrada()
        
    Funciones desde donde es llamado:

    Parámetros de entrada:
        + const char *camino
        + unsigned char permisos

    Parámetros de salida:
        + 0: Ejecución correcta
        + (-1): Error
*/
int mi_creat(const char *camino, unsigned char permisos) {
    //Declaraciones
    struct superbloque SB;
    int result;
    int posInodoRaiz, p_inodo, p_entrada;

    //Lectura de superbloque para obtener posición de inodo raiz
    if(bread(posSB, &SB) == -1) {
        fprintf(stderr, "Error en lectura de superbloque. "
        "Función -> mi_creat()");
    }

    posInodoRaiz = SB.posInodoRaiz;

    result = buscar_entrada(camino, &posInodoRaiz, &p_inodo, &p_entrada, 1, permisos);

    //Controlar los posibles errores desde result
    //Todavía por definir la diferenciación entre errores

}

/*
    Descripción:    
        Función que copia el contenido de un directorio en un buffer 

    Funciones a las que llama:
        + bloques.h - bread()
        + ficheros.h - mi_stat_f()
        + ficheros.h - mi_read_f()
        + directorios.h - buscar_entrada()
        
    Funciones desde donde es llamado:

    Parámetros de entrada:
        + const char *camino
        + unsigned char permisos

    Parámetros de salida:
        + 0: Ejecución correcta
        + (-1): Error
*/
int mi_dir(const char *camino, char *buffer) {
    //Declaraciones
    struct superbloque SB;
    int result;
    int posInodoRaiz, p_inodo, p_entrada;
    struct STAT stat, s_aux;

    //Buffer para lecturas de entradas
    struct entrada entradas[BLOCKSIZE/sizeof(struct entrada)];
    int index = 0;      //Iterar sobre buffer
    int limit = sizeof(entradas)/sizeof(entradas[0]);   //Limite de iterador
    int offset = 0;
    //Inicializar buffer a 0's
    memset(entradas, 0, sizeof(entradas));

    //Lectura de superbloque para obtener posición de inodo raiz
    if(bread(posSB, &SB) == -1) {
        fprintf(stderr, "Error en lectura de superbloque. "
        "Función -> mi_creat()");
    }

    posInodoRaiz = SB.posInodoRaiz;

    //Obtener inodo correspondiente a directorio
    result = buscar_entrada(camino, &posInodoRaiz, &p_inodo, &p_entrada, 0, 7);

    /*
        TRATAR ERRORES
    */

    //Lectura de inodo
    if (mi_stat_f(p_inodo, &stat) == -1) {
        return fprintf(stderr,"Error en lectura de inodo. "
        "Función -> mi_dir()");
        return -1;
    }

    //Comprobar que inodo sea de tipo directorio
    if (stat.tipo != 'd') {
        fprintf(stderr,"Error: se está intentando leer entradas de directorio desde"
        " un fichero.\n");
        return -1;
    }

    //Comprobar que inodo tenga permisos de lectura
    if ((stat.permisos & 4) != 4) {
        fprintf(stderr, "Error: se está intentando leer de un directorio sin "
        "permisos.\n");
        return -1;
    }

    //Leer primer bloque de entradas
    if (mi_read_f(p_inodo, entradas, offset, sizeof(entradas)) == -1) {
        fprintf(stderr, "Error: imposible leer desde disco. "
        "Función -> mi_dir()");
        return -1;
    }

    //Recorrido a buffer de entradas
    while (entradas[index].ninodo != 0 && index < limit) {
        //Leer información sobre el inodo
        if (mi_stat_f(entradas[index].ninodo, &s_aux) == -1) {
            //Concatenar únicamente nombre de entrada
            strcat(buffer, entradas[index].nombre);
            strcat(buffer, "    |");
            //Enviar mensaje de error por stderr
            fprintf(stderr, "Error: no se ha podido leer inodo de la entrada.\n");

        } else {
            //Concatenar información sobre tipo
            strcat(buffer, s_aux.tipo + "   ");

            //Concatenar información sobre permisos de lectura
            if ((s_aux.permisos & 4) == 4) {
                strcat(buffer, "r");
            } else {
                strcat(buffer, "-");
            }
            //Concatenar información sobre permisos de escritura
            if ((s_aux.permisos & 2) == 2) {
                strcat(buffer, "w");
            } else {
                strcat(buffer, "-");
            }
            //Concatenar información sobre permisos de ejecución
            if ((s_aux.permisos & 1) == 1) {
                strcat(buffer, "x");
            } else {
                strcat(buffer, "-");
            }

            strcat(buffer, "    "); //Tabulación entre columnas

            //Concatenar nombre de entrada
            strcat(buffer, entradas[index].nombre && "   ");

            //Formatear información de mtime de inodo
            struct tm *tm; //ver info: struct tm
            char tmp[100];
            tm = localtime(&stat.mtime);
            sprintf(tmp,"%d-%02d-%02d %02d:%02d:%02d\t",tm->tm_year+1900,
                    tm->tm_mon+1,tm->tm_mday,tm->tm_hour,tm->tm_min,tm->tm_sec);
            //Concatenar información de mtime
            strcat(buffer,tmp);

            //Concatenar tabulador y separador de entrada
            strcat(buffer, "    |");
        }

        index++;

        //Chequear si hay que leer de nuevo desde disco
        if (index == limit) {
            offset += sizeof(entradas);
            if (mi_read_f(p_inodo, entradas, offset, sizeof(entradas)) == -1) {
                fprintf(stderr, "Error: imposible leer desde disco. "
                "Función -> mi_dir()");
                return -1;
            }
            index = 0;
            //Inicializar buffer a 0's
            memset(entradas, 0, sizeof(entradas));
        }
    }

    return 0;
}

/*
    Descripción:    
        Función que busca el inodo correspondiente a la entrada especificada en
        camino y cambia sus permisos.

    Funciones a las que llama:
        + bloques.h - bread()
        + ficheros.h - mi_chmod_f()
        + directorios.h - buscar_entrada()
        
    Funciones desde donde es llamado:

    Parámetros de entrada:
        + const char *camino
        + unsigned char permisos

    Parámetros de salida:
        + 0: Ejecución correcta
        + (-1): Error
*/
int mi_chmod(const char *camino, unsigned char permisos) {
    //Declaraciones
    struct superbloque SB;
    int result;
    int posInodoRaiz, p_inodo, p_entrada;

    //Lectura de superbloque para obtener posición de inodo raiz
    if(bread(posSB, &SB) == -1) {
        fprintf(stderr, "Error en lectura de superbloque. "
        "Función -> mi_creat()");
    }

    posInodoRaiz = SB.posInodoRaiz;

    //Obtener inodo correspondiente a directorio
    result = buscar_entrada(camino, &posInodoRaiz, &p_inodo, &p_entrada, 0, 7);

    /*
        TRATAR ERRORES
    */

   //Cambiar permisos con mi_chmod_f
   if (mi_chmod_f(p_inodo, permisos) == -1) {
       fprintf(stderr, "Error: no se ha podido modificar permisos. "
       "Función -> mi_chmod()\n");
       return -1;
   }

   return 0;
}

/*
    Descripción:    
        Función que obtiene información particular del inodo de la entrada
        indicada por camino.

    Funciones a las que llama:
        + bloques.h - bread()
        + ficheros.h - mi_stat_f()
        + directorios.h - buscar_entrada()
        
    Funciones desde donde es llamado:

    Parámetros de entrada:
        + const char *camino
        + unsigned char permisos

    Parámetros de salida:
        + 0: Ejecución correcta
        + (-1): Error
*/
int mi_stat(const char *camino, struct STAT *p_stat) {
    //Declaraciones
    struct superbloque SB;
    int result;
    int posInodoRaiz, p_inodo, p_entrada;

    //Lectura de superbloque para obtener posición de inodo raiz
    if(bread(posSB, &SB) == -1) {
        fprintf(stderr, "Error en lectura de superbloque. "
        "Función -> mi_creat()");
    }

    posInodoRaiz = SB.posInodoRaiz;

    //Obtener inodo correspondiente a directorio
    result = buscar_entrada(camino, &posInodoRaiz, &p_inodo, &p_entrada, 0, 7);

    /*
        TRATAR ERRORES
    */

    //Imprimir por pantalla el número de inodo ----- NIVEL 9
    printf("Nº de inodo: %i\n", p_inodo);

    //Obtención de datos
    if (mi_stat_f(p_inodo, p_stat) == -1) {
        fprintf(stderr, "Error: no se ha podido obtener el estatus del inodo."
        " Función -> mi_stat()");
        return -1;
    }

    return 0;
}
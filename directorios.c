#include <string.h>
#include "directorios.h"
#include "bloques.h"

#define BUF_SIZE BLOCKSIZE
#define NAME_MAX_LENGTH 60     //EXT2 filename size limit
struct UltimaEntrada UltimaEntradaLectura;

/*----------------------------FUNCIONES DE NIVEL 8----------------------------*/

/*
    Descripción:
        Dada una cadena de caracteres camino (que comience por '/'), 
        separa su contenido en dos.
        
        Ejemplos:
            camino = "/dir1/dir2/fichero"
            inicial = "dir1" (devuelve DIRECTORIO)
            final = "/dir2/fichero"

    Funciones desde donde es llamado:
        + directorios.h - buscar_entrada()

    Parámetros de entrada:
        + const char *camino
        + char *inicial
        + char *final
        + char *tipo 

    Parámetros de salida:
        + 1: La ruta corresponde a un directorio
        + 0: La ruta corresponde a un fichero
        + (-1): Ruta inválida
*/
int extraer_camino(const char *camino, char *inicial, char *final, char *tipo) {
   
    //Devuelve -1 en caso de que el camino no comience por el carácter '/'
    if (camino[0] != '/'){
        return -1;             
    }
    int i = 1;

    /*
    En el siguiente bucle guardaremos en inicial la porción comprendida entre
    los dos primeros '/' o en el caso que no haya un segundo '/' guarda camino 
    en inicial también.
    */
    while ((camino[i] != '/') && (camino[i] != '\0')){  
        inicial[i-1] = camino[i];
        i++;
    }

    inicial[i-1] = '\0'; //Marcar fin de String.
    
    //Devolvemos 0 por que lo que hay en el camino corresponde a un fichero.
    if (i == strlen(camino)){
        *final = '\0';
        *tipo = 'f';
        return 0;               
    }

    int j = 0;
    while (i < strlen(camino)){
        final[j] = camino[i];
        i++;
        j++;
    }
    *tipo = 'd';
    final[j] = '\0';
    
    //Devolvemos 1 por quelo que hay en el camino corresponde a un directorio.
    return 1;                   
}

/*
    Descripción:    
        Como su nombre indica, esta función nos buscará la entrada indicada, 
        entre las entradas del inodo correspondiente a su directorio padre.

    Funciones a las que llama:
        + ficheros_basico.h - leer_inodo()
        + ficheros_basico.h - reservar_inodo()
        + ficheros_basico.h - liberar_inodo()
        + ficheros.h - mi_read_f()
        + ficheros.h - mi_write_f()
        + directorios.h - extraer_camino()
        + directorios.h - buscar_entrada()
        
    Funciones desde donde es llamado:
        + directorios.h - buscar_entrada()
        + directorios.h - mi_creat()
        + directorios.h - mi_dir()
        + directorios.h - mi_chmod()
        + directorios.h - mi_stat();
        + directorios.h - mi_touch(); 

    Parámetros de entrada:
        + const char *camino_parcial
        + unsigned int *p_inodo_dir
        + unsigned int *p_inodo
        + unsigned int *p_entrada
        + char reservar
        + unsigned char permisos

    Parámetros de salida:
        + 0: Ejecución correcta. 
        + (-2): Error: El inodo no se ha podido leer correctamente. 
        + (-3): Error: Permiso denegado de lectura. 
        + (-4): Error: No existe el archivo o directorio. 
        + (-5): Error: Permiso denegado de escritura. 
        + (-6): Error: No existe algún directorio intermedio. 
        + (-7): Error: EXIT_FAILURE.
        + (-8): Error: No se puede crear una entrada en un fichero. 
        + (-9): Error: Entrada ya existente.
        + (-10): Error: Camino incorrecto.  
        ------------------------------------------------------------------------------------> falta decir que modifica los parametros que el entran
*/
int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo_dir, unsigned int *p_inodo, unsigned int *p_entrada, char reservar, unsigned char permisos) {
    //Declaraciones
    char inicial[NAME_MAX_LENGTH]; 
    char final[NAME_MAX_LENGTH]; 
    char tipo; 
    struct inodo inodo_dir;

    //Caso en el que sólo se pasa la raíz. 
    if (strcmp(camino_parcial, "/") == 0) {
        *p_inodo = 0; 
        *p_entrada = 0;
        return 0; 
    }

    //inicializar variables
    memset(inicial, 0, NAME_MAX_LENGTH);
    memset(final, 0, NAME_MAX_LENGTH);

    if (extraer_camino(camino_parcial, inicial, final, &tipo)==-1) {
        return -10; 
    }

    //Buscamos la entrada cuyo nombre se encuentra en la inicial. 
    if (leer_inodo(*p_inodo_dir, &inodo_dir) == -1) {
        return -2;
    }
    
    if ((inodo_dir.permisos & 4)!=4) {
        return -3; 
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
    int ninodo = 0; 

    //Poner buffers a 0's
    //memset(entrada.nombre, 0, sizeof(entrada.nombre));
    memset(entradas, 0, BLOCKSIZE);

    //Calcular el número de entradas del inodo (numentradas).
    numentradas = inodo_dir.tamEnBytesLog/sizeof(struct entrada);


    //Utilizar buffer de tamaño blocksize para las entradas y reducir accesos
    if (numentradas > 0) {

        //Leer un blocksize de entradas
        mi_read_f(*p_inodo_dir, entradas, offset, BLOCKSIZE);
        
        while ((nentrada < numentradas) &&
               (strcmp(inicial, entradas[index].nombre) != 0)) {
            nentrada++;
            index++;
            //Guardar offset para posterior escritura
            offset += sizeof(struct entrada);   

            if (index == BLOCKSIZE/sizeof(struct entrada)) {
                index = 0;
                //Leer siguiente bloque de entradas
                mi_read_f(*p_inodo_dir, entradas, offset, BLOCKSIZE);
            }
        }
    }

    if (nentrada==numentradas) { //Se ha recorrido el total de entradas

        switch (reservar) {

            //Modo consulta. Como no existe retornamos error. 
            case 0: 

                return -4; 
                break; 

            //Modo escritura. 
            case 1: 
                if (inodo_dir.tipo == 'f') {
                    return -8;
                }

                //Si es directorio comprobar que tiene permiso de escritura. 
                if ((inodo_dir.permisos & 2)!=2) {
                    return -5;
                    
                }else{

                    //Copiar inicial en el nombre de la entrada. 
                    strcpy(entradas[index].nombre, inicial);
        
                    if (tipo == 'd') {
                        if(!strcmp(final, "/")) { //resultado esperado == 0
                            ninodo = reservar_inodo(tipo, permisos);
                            entradas[index].ninodo = ninodo; 
                        }else{
                            //Cuelgan más directorios o ficheros. 
                             return -6; 
                        }

                    }else{
                        //Es un fichero
                        ninodo = reservar_inodo('f', permisos); 

                        if (ninodo == -1) {
                            fprintf(stderr, "Error: No se ha podido reservar el inodo. Función -> buscar_entrada()\n"); 
                        }

                        entradas[index].ninodo = ninodo; 
                    }

                    if (mi_write_f(*p_inodo_dir, &entradas[index], offset, sizeof(struct entrada))==-1) {
                        
                        if (entradas[index].ninodo!=-1) {
                            if (liberar_inodo(entradas[index].ninodo)==-1) {
                                fprintf(stderr, "Error: No se ha podido liberar el inodo. Función -> buscar_entrada()\n");
                            }
                        }
                        return -7;
                }
            }
        }
    }

    if (!strcmp(final, "\0") || !strcmp(final, "/")) {
        if ((nentrada < numentradas) && (reservar==1)) {
            //Modo escritura y la entrada ya existe.
            return -9; 
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
        Función que crea un directorio y su entrada de directorio

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

    mi_waitSem();

    //Declaraciones
    struct superbloque SB;
    int result;
    unsigned int posInodoRaiz = 0, p_inodo=0, p_entrada=0;

    //Comprobación de que el último carácter sea '/' (creación de fichero).
    if (camino [strlen(camino) - 1] != '/'){    
    fprintf(stderr, "Error: Para crear ficheros se tiene que usar el comando mi_touch.\n");
    mi_signalSem();
    exit(-1);
    }    

    //Lectura de superbloque para obtener posición de inodo raiz
    if(bread(posSB, &SB) == -1) {
        fprintf(stderr, "Error en lectura de superbloque. "
        "Función -> mi_creat()\n");
    }

    posInodoRaiz = SB.posInodoRaiz;

    result = buscar_entrada(camino, &posInodoRaiz, &p_inodo, &p_entrada, 1, permisos);

    if (result < 0) {
        mi_signalSem();
        return result; 
    }

    mi_signalSem();
    
    return 0;
}

/*
    Descripción:    
        Función que copia el contenido de un directorio en un buffer.
        Esta función separa la información de las entradas con "_" y 
        separa las entradas con "|".


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
    unsigned int posInodoRaiz = 0, p_inodo, p_entrada;
    struct STAT stat, s_aux;
    char str[12];

    //Buffer para lecturas de entradas
    struct entrada entradas[BLOCKSIZE/sizeof(struct entrada)];
    int index = 0;      //Iterar sobre buffer
    int limit = sizeof(entradas)/sizeof(entradas[0]);   //Limite de iterador
    int offset = 0;
    int nentradas = 0;

    //Inicializar buffer a 0's
    memset(entradas, 0, sizeof(entradas));
    memset(str, 0, sizeof(str));

    //Lectura de superbloque para obtener posición de inodo raiz
    if(bread(posSB, &SB) == -1) {
        fprintf(stderr, "Error en lectura de superbloque. "
        "Función -> mi_dir()\n");
    }

    posInodoRaiz = SB.posInodoRaiz;

    //Obtener inodo correspondiente a directorio
    result = buscar_entrada(camino, &posInodoRaiz, &p_inodo, &p_entrada, 0, 6);

    if (result < 0) {
        return result;
    }

    //Lectura de inodo
    if (mi_stat_f(p_inodo, &stat) == -1) {
        return fprintf(stderr,"Error en lectura de inodo. "
        "Función -> mi_dir()\n");
        return -1;
    }

    //Comprobar que el directorio no esté vacío
    if (stat.tamEnBytesLog == 0) {
        return 0;   //No llena el buffer
    }

    //Obtener Nº de entradas del directorio
    nentradas = stat.tamEnBytesLog/sizeof(struct entrada);

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
        "Función -> mi_dir()\n");
        return -1;
    }

    result = 0; //Contador de entradas
    //Recorrido a buffer de entradas
    while (result < nentradas && index < limit) {
        //Leer información sobre el inodo
        if (mi_stat_f(entradas[index].ninodo, &s_aux) == -1) {
            //Concatenar únicamente nombre de entrada
            strcat(buffer, entradas[index].nombre);
            strcat(buffer, "|");
            //Enviar mensaje de error por stderr
            fprintf(stderr, "Error: no se ha podido leer inodo de la entrada.\n");

        } else {
            //Concatenar información sobre tipo
            sprintf(str,"%c", s_aux.tipo);
            strcat(buffer, str);
            strcat(buffer, "      ");

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

            strcat(buffer, "        "); //Tabulación entre columnas

            //Formatear información de mtime de inodo
            struct tm *tm; //ver info: struct tm
            char tmp[100];
            tm = localtime(&s_aux.mtime);
            sprintf(tmp,"%d-%02d-%02d %02d:%02d:%02d\t",tm->tm_year+1900,
                    tm->tm_mon+1,tm->tm_mday,tm->tm_hour,tm->tm_min,tm->tm_sec);
            //Concatenar información de mtime
            strcat(buffer,tmp);
            strcat(buffer, "    ");
            
            //Concatenar tamaño del directorio/fichero
            sprintf(str,"%d",s_aux.tamEnBytesLog);
            strcat(buffer, str);
            strcat(buffer, "    ");

            //Concatenar nombre de entrada
            strcat(buffer, entradas[index].nombre);

            //Concatenar tabulador y separador de entrada
            strcat(buffer, "|");
        }

        index++;
        result++;

        //Chequear si hay que leer de nuevo desde disco
        if (index == limit) {
            offset += sizeof(entradas);
            if (mi_read_f(p_inodo, entradas, offset, sizeof(entradas)) == -1) {
                fprintf(stderr, "Error: imposible leer desde disco. "
                "Función -> mi_dir()\n");
                return -1;
            }
            index = 0;
        }
    }

    return result;
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
    unsigned int posInodoRaiz = 0, p_inodo, p_entrada;

    //Lectura de superbloque para obtener posición de inodo raiz
    if(bread(posSB, &SB) == -1) {
        fprintf(stderr, "Error en lectura de superbloque. "
        "Función -> mi_chmod()\n");
        exit(-1);
    }

    posInodoRaiz = SB.posInodoRaiz;

    //Obtener inodo correspondiente a directorio
    result = buscar_entrada(camino, &posInodoRaiz, &p_inodo, &p_entrada, 0, 7);
    
    if (result < 0) {
        return result; 
    }

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
    unsigned int posInodoRaiz = 0, p_inodo, p_entrada;

    //Lectura de superbloque para obtener posición de inodo raiz
    if(bread(posSB, &SB) == -1) {
        fprintf(stderr, "Error en lectura de superbloque. "
        "Función -> mi_stat()\n");
    }

    posInodoRaiz = SB.posInodoRaiz;

    //Obtener inodo correspondiente a directorio
    result = buscar_entrada(camino, &posInodoRaiz, &p_inodo, &p_entrada, 0, 7);
        
    //printf("Nº inodo = %d\n", p_inodo);

    if (result < 0) {
        return result;  
    }

    //Obtención de datos
    if (mi_stat_f(p_inodo, p_stat) == -1) {
        fprintf(stderr, "Error: no se ha podido obtener el estatus del inodo."
        " Función -> mi_stat()\n");
        return -1;
    }

    return 0;
}

/*------------------------FUNCIÓN OPCIONAL NIVEL 9----------------------------*/
/*
    Descripción:    
        Función que crea un fichero y su entrada de fichero

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
int mi_touch(const char *camino, unsigned char permisos) {

    //Declaraciones
    struct superbloque SB;
    int result;
    unsigned int posInodoRaiz = 0, p_inodo, p_entrada;
   
    if (camino [strlen(camino) - 1] == '/'){    
    fprintf(stderr, "Error: Para crear directorios se tiene que usar el comando mi_mkdir.\n");
    exit(-1);
    }    

    //Lectura de superbloque para obtener posición de inodo raiz
    if(bread(posSB, &SB) == -1) {
        fprintf(stderr, "Error en lectura de superbloque. "
        "Función -> mi_creat()\n");
    }

    posInodoRaiz = SB.posInodoRaiz;

    result = buscar_entrada(camino, &posInodoRaiz, &p_inodo, &p_entrada, 1, permisos);

    if (result < 0) {
        return result; 
    }
    return 0;
}

/*
    Descripción:    
        Función para diferenciar todos los tipos de errores que puede generar
        la función buscar_entrada. 
        
    Funciones desde donde es llamado:
        + directorios.h - mi_creat()
        + directorios.h - mi_dir()
        + directorios.h - mi_chmod()
        + directorios.h - mi_stat();
        + directorios.h - mi_touch(); 

    Parámetros de entrada:
        + int nerror: número de error de buscar_entrada.
        + char *buffer. 

    Parámetros de salida:
        + 0: Ejecución correcta
        + (-1): Error
*/
int control_errores_buscar_entrada(int nerror, char *buffer) {
    
    switch (nerror) {

        case -10: 
        strcpy(buffer, "Error: Camino incorrecto.\n");
        break; 

        case -2: 
        strcpy(buffer, "Error: El inodo no se ha podido leer correctamente.\n");
        break; 

        case -3:  
        strcpy(buffer, "Error: Permiso denegado de lectura.\n");
        break; 

        case -4: 
        strcpy(buffer, "Error: No existe el archivo o el directorio.\n");
        break; 

        case -5: 
        strcpy(buffer, "Error: Permiso denegado de escritura\n");
        break; 

        case -6: 
        strcpy(buffer, "Error: No existe algún directorio intermedio\n");
        break; 

        case -7: 
        strcpy(buffer, "EXIT_FAILURE\n");
        break; 

        case -8: 
        strcpy(buffer, "Error: No se puede crear entrada en un fichero\n");
        break; 

        case -9: 
        strcpy(buffer, "Error: Entrada ya existente\n");
    }
    return 0; 
}

/*----------------------------FUNCIONES NIVEL 10------------------------------*/

/*
    Descripción:    
        Función de directorios.c para leer los nbytes del fichero indicado por 
        camino, a partir del offset pasado por parámetro y copiarlos en un 
        buffer.
        
    Funciones desde donde es llamado:
        + mi_cat.c - main()

    Parámetros de entrada:
        + const char *camino
        + void *buf
        + unsigned int offset
        + unsigned int nbytes

    Parámetros de salida:
        + Número de bytes leídos. 
        + <0: Algún error ocurrido. 
*/
int mi_read(const char *camino, void *buf, unsigned int offset, unsigned int nbytes) {

    //Declaraciones. 
    int bytesleidos;
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    int buscar_ent = 0;

    //Verificación inicial para no tener que llamar a buscar_entrada. 
    if (strcmp(camino, UltimaEntradaLectura.camino)==0) {

        p_inodo = UltimaEntradaLectura.p_inodo;

    } else {

        buscar_ent = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 6);

        //Actualización de la caché de lectura. 
        UltimaEntradaLectura.p_inodo = p_inodo; 
        strcpy(UltimaEntradaLectura.camino, camino);
    }

    //Error durante la lectura. Se pasa el error a la función origen. 
    if (buscar_ent < 0){
        return buscar_ent;
    }

    //Se realiza la lectura y se guarda la cantidad de bytes leídos. 
    bytesleidos = mi_read_f(p_inodo, buf, offset, nbytes);

    //Error en mi_read_f()
    if (bytesleidos < 0){
        return -1;
    }

    return bytesleidos;
}

/*
    Descripción:    
        Función de directorios.c para escribir contenido en un fichero.
        Buscaremos la entrada camino con buscar_entrada() para obtener 
        el p_inodo.
        
    Funciones desde donde es llamado:
        + mi_escribir.c - main()

    Parámetros de entrada:
        + const char *camino
        + void *buf
        + unsigned int offset
        + unsigned int nbytes

    Parámetros de salida:
        + Número de bytes escritos. 
        + <0: Algún error ocurrido. 
*/
int mi_write (const char *camino, const void *buf, unsigned int offset, unsigned int nbytes) {

    //Declaraciones.
    int bytesEscritos;    
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    int buscar_ent = 0; 

    //Comprobación para no tener que llamar cada vez a la función buscar_entrada. 
    if (strcmp(camino, UltimaEntradaLectura.camino)==0) {

        p_inodo = UltimaEntradaLectura.p_inodo;

    } else {
        buscar_ent = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 6);

        //Actualización de la caché de escritura. 
        UltimaEntradaLectura.p_inodo = p_inodo; 
        strcpy(UltimaEntradaLectura.camino, camino);
    }

    //Algúin error ocurrido en buscar_entrada. Se tratará en la función origen. 
    if (buscar_entrada < 0){
        return buscar_ent;
    }

    bytesEscritos = mi_write_f(p_inodo, buf, offset, nbytes);

    //Error en mi_write_f()
    if (bytesEscritos < 0){
        return -1;
    }

    return bytesEscritos;
}

/*----------------------------FUNCIONES NIVEL 11------------------------------*/

/*
    Descripción:
        Función que enlaza un fichero a crear sobre la ruta camino2 a la ruta
        camino1.

    Funciones a las que llama:
        + ficheros_basico.h - leer_inodo()
        + ficheros_basico.h - escribir_inodo()
        + ficheros_basico.h - liberar_inodo()
        + ficheros.h - mi_write_f()
        + ficheros.h - mi_read_f()
        + directorios.h - buscar_entrada()
        
    Funciones desde donde es llamado:

    Parámetros de entrada:
        + const char *camino1
        + const char *camino2

    Parámetros de salida:
        + 0: Ejecución correcta
        + (-1): Error
*/
int mi_link(const char *camino1, const char *camino2) {
    
    mi_waitSem();

    //Declaraciones
    struct inodo inodo1, inodo2;
    struct entrada entrada;
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo1 = 0, p_inodo2 = 0;
    unsigned int p_entrada1 = 0, p_entrada2 = 0;
    int buscar_ent;
    
    //Comprobar que el fichero al que apunta camino1 exista
    buscar_ent = buscar_entrada(camino1, &p_inodo_dir, &p_inodo1, &p_entrada1, 0, 6);
    
    //Recuperación de errores de buscar entrada en el programa origen. 
    if (buscar_ent < -1) {
        mi_signalSem();
        return buscar_ent;
    }

    /*Comprobar el inodo*/
    //Lectura de inodo
    if (leer_inodo(p_inodo1, &inodo1) < 0) {
        fprintf(stderr, "Error: lectura de inodo fallida. Función -> mi_link()\n");
        mi_signalSem();
        return -1;
    }

    //Comprobar que sea fichero
    if (inodo1.tipo != 'f') {
        fprintf(stderr, "Error: no se puede enlazar una ruta a un directorio. "
        "Función -> mi_link()\n");
        mi_signalSem();
        return -1;
    }

    //Comprobar que se tenga permisos de lectura
    if ((inodo1.permisos & 4) != 4) {
        fprintf(stderr, "Error: no se tiene permisos de lectura para la ruta "
        "a enlazar. Función -> mi_link()\n");
        mi_signalSem();
        return -1;
    }

    //Comprobar que el fichero al que apunta camino2 no exista
    p_inodo_dir = 0;
    buscar_ent = buscar_entrada(camino2, &p_inodo_dir, &p_inodo2, &p_entrada2, 1, 6);
    
    //Recuperación de errores de buscar entrada en el programa origen. 
    // -4 es el código de error de fichero no existente de la función buscar 
    //entrada. 

    if (buscar_ent < 0) {
        mi_signalSem();
        return buscar_ent;
    }

    /*Comprobar el inodo*/
    //Lectura de inodo
    if (leer_inodo(p_inodo2, &inodo2) < 0) {
        fprintf(stderr, "Error: lectura de inodo fallida. Función -> mi_link()\n");
        mi_signalSem();
        return -1;
    }
    
    //Comprobar que sea fichero
    if (inodo2.tipo != 'f') {
        fprintf(stderr, "Error: no se puede enlazar una ruta de un directorio "
        "a la ruta origen. Función -> mi_link()\n");
        mi_signalSem();
        return -1;
    }

    //Comprobar que se tenga permisos de lectura
    if ((inodo2.permisos & 4) != 4) {
        fprintf(stderr, "Error: no se tiene permisos de lectura para la ruta "
        "a enlazar. Función -> mi_link()\n");
        mi_signalSem();
        return -1;
    }

    //Lectura de la entrada dentro del inodo
    if (mi_read_f(p_inodo_dir, &entrada, p_entrada2*sizeof(struct entrada), 
    sizeof(struct entrada)) == -1) {
        fprintf(stderr, "Error: imposible leer entrada de ruta a enlazar. "
        "Función -> mi_link()\n");
        mi_signalSem();
        return -1;
    }

    //Modificar puntero al inodo de la entrada
    entrada.ninodo = p_inodo1;

    //Escritura de la entrada dentro del inodo
    if (mi_write_f(p_inodo_dir, &entrada, p_entrada2*sizeof(struct entrada),
    sizeof(struct entrada)) == -1) {
        fprintf(stderr, "Error: imposible escribir entrada de ruta a enlazar. "
        "Función -> mi_link()\n");
        mi_signalSem();
        return -1;
    }

    //Modificar número de enlaces del inodo origen
    inodo1.nlinks++;
    inodo1.ctime = time(NULL);

    //Guardar inodo
    if (escribir_inodo(p_inodo1, inodo1) == -1) {
        fprintf(stderr, "Error: imposible salvar inodo de ruta origen. "
        "Función -> mi_link()\n");
        mi_signalSem();
        return -1;
    }

    //Liberar inodo del camino a enlazar
    if (liberar_inodo(p_inodo2) == -1) {
        fprintf(stderr, "Error: no se ha podido liberar el inodo de la ruta a enlazar."
        " Función -> mi_link()\n");
        mi_signalSem();
        return -1;
    }

    mi_signalSem();
    return 0;
}

/*
    Descripción: 
        Borra la entrada de directorio especificada (no hay que olvidar 
        actualizar la cantidad de enlaces en el inodo) y, en caso de que fuera 
        el último enlace existente, borrar el propio fichero/directorio.

    Funciones a las que llama:
        + ficheros_basico.h - leer_inodo()
        
    Funciones desde donde es llamado:

    Parámetros de entrada:
        + const char *camino

    Parámetros de salida:
        + 0: Ejecución correcta
        + (-1): Error
*/
int mi_unlink(const char *camino) {

    //Mutex lock
    mi_waitSem();

    //Declaraciones
    unsigned int p_inodo_dir = 0, p_inodo = 0, p_entrada = 0; 
    int buscar_ent; 
    struct inodo inodo, inodo2;
    int nentradas;
    struct entrada entrada;
    char buff[256]; 

    buscar_ent = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 6);

    //Errores de la función buscar_entrada. 
    if (buscar_ent < 0) {
        control_errores_buscar_entrada(buscar_ent, buff);
        fprintf(stderr, "%s", buff);
        mi_signalSem(); //mutex unlock
        exit(-1);
    }

    //Lectura del inodo. 
    if (leer_inodo(p_inodo, &inodo)==-1) {
        fprintf(stderr, "Error: La lectura del inodo ha fallado. Función -> mi_unlink()\n");
        mi_signalSem(); //mutex unlock
        return -1;
    }

    //Comprobación que si es directorio no este vacío. 
    if ((inodo.tipo=='d') && (inodo.tamEnBytesLog > 0)) {
        fprintf(stderr, "Error: El directorio no esta vacío. Función -> mi_unlink()\n");
        mi_signalSem(); //mutex unlock
        return -1;
    }

    //Lectura de p_inodo_dir (inodo del directorio que contiene la entrada que 
    //se desea eliminar). 
    if (leer_inodo(p_inodo_dir, &inodo2)==-1) {
        fprintf(stderr, "Error: La lectura del inodo padre ha fallado. Función -> mi_unlink()\n");
        mi_signalSem(); //mutex unlock
        return -1;    
    } 

    //Calcular el número de entradas que tiene el directorio. 
    nentradas = inodo2.tamEnBytesLog/sizeof(struct entrada); 

    //Caso que la entrada a quitar sea la última. 
    if (p_entrada == (nentradas -1)) {
        
        //Comprobación de errores de la función mi_truncar_f
        if (mi_truncar_f(p_inodo_dir, inodo2.tamEnBytesLog-sizeof(struct entrada))==-1) {
            fprintf(stderr, "Error: No se ha podido truncar el inodo. Función -> mi_unlink()\n");
            mi_signalSem(); //mutex unlock
            return -1;
        }

    } else {
        //Caso que la entrada a quitar no sea la última. 

        //Lectura de la última entrada. 
        if (mi_read_f(p_inodo_dir, &entrada, inodo2.tamEnBytesLog-sizeof(struct entrada), sizeof(struct entrada))==-1) {
            fprintf(stderr, "Error: No se ha podido leer la última entrada. Función -> mi_unlink()\n");
            mi_signalSem(); //mutex unlock
            return -1;
        }

        //Sobreescribir última entrada en la posición de la entrada a borrar
        if (mi_write_f(p_inodo_dir, &entrada, p_entrada*sizeof(struct entrada), sizeof(struct entrada))==-1) {
            fprintf(stderr, "Error: No se ha podido escribir la entrada. Función -> mi_unlink()\n");
            mi_signalSem(); //mutex unlock
            return -1;
        }

        //Truncar el inodo en la última entrada. (Tras el intercambio, la 
        //entrada que se quiere eliminar esta en la última posición). 
        if (mi_truncar_f(p_inodo_dir, inodo2.tamEnBytesLog-sizeof(struct entrada))==-1) {
            fprintf(stderr, "Error: No se ha podido truncar el inodo. Función -> mi_unlink()\n");
            mi_signalSem(); //mutex unlock
            return -1;
        }
    }

    //Reducir en una unidad el número de links del inodo del fichero
    inodo.nlinks--;

    //Si no quedan enlaces libres se libera el inodo. 
    if (inodo.nlinks == 0) {
        //Librerar inodo. 
        if (liberar_inodo(p_inodo)==-1) {
            fprintf(stderr, "Error: No se ha podido liberar el inodo. Función -> mi_unlink()\n");
            mi_signalSem(); //mutex unlock
            return -1;    
        } 

    } else {
        //Actualización de ctime.
        inodo.ctime = time(NULL);

        //Escritura del inodo. 
        if (escribir_inodo(p_inodo, inodo)==-1) {
            fprintf(stderr, "Error: No se ha podido escribir el inodo. Función -> mi_unlink()\n");
            mi_signalSem(); //mutex unlock
            return -1;    
        } 
    }
    //mutex unlock
    mi_signalSem();
    return 0;
}
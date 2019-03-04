#include "ficheros_basico.h"
#include <limits.h>

/*FUNCIONES DE NIVEL 2*/

/*
    Descripción:
        Función que calcula el número de bloques necesarios para el mapa de
        bits.
        Lo hace mediante la fórmula: (Número de bloques / 8 bits) / BLOCKSIZE.

    Funciones desde donde es llamado:
        + mi_mkfs.c - main()

    Parámetros de entrada:
        + nbloques: El número de bloques que tiene el sistema de ficheros
        int temp;
	
    Parámetros de salida:
        + Número de bloques necesario para el mapa de bits.
*/
int tamMB(unsigned int nbloques){

    if (((nbloques / 8) % BLOCKSIZE) != 0){
        /*El modulo es diferente de 0 y por lo tanto hemos de añadir
        un bloque adicional para los bytes (el resto de la división)*/
        return ((nbloques / 8) / BLOCKSIZE) + 1;

    } else{ 
        //El modulo es igual a 0 por lo tanto el numero de bloques será exacto
        return ((nbloques / 8) / BLOCKSIZE); 
    }
}

/*
    Descripción:
        Función que calcula el número en bloques necesarios para los inodos que 
        tendrá el sistema de ficheros.
        Lo hace mediante la fórmula: (Número de inodos / INODOSIZE) / BLOCKSIZE

    Funciones desde donde es llamado:
        + mi_mkfs.c - main()

    Parámetros de entrada:
        + ninodos: El número de inodos que tiene el sistema de ficheros.
    
    Parámetros de salida:
        + Número de bloques necesarios para el número de inodos del sistema.
*/
int tamAI(unsigned int ninodos){

    if (((ninodos * INODOSIZE) % BLOCKSIZE) != 0){
        /*El modulo es diferente de 0 y por lo tanto necesitamos
        añadir un bloque adicional para los bytes (resto de la división)*/
        return ninodos * INODOSIZE / BLOCKSIZE + 1; 
                                                    
    }
    else{
        //El modulo es igual a 0 por lo que el número de bloque es exacto.
        return ninodos * INODOSIZE / BLOCKSIZE; 
    }
}

/*
    Descripción:
        Función que inicializa los metadados contenidos en el superbloque a
        partir de las funciones tamMB() y tamAI(). 
        Posteriormente, escribe el superbloque inicializado en el bloque posSB
        del sistema de ficheros.
    
    Funciones a las que llama:
        + bloques.h - bwrite()
    
    Funciones desde donde es llamado:
        + mi_mkfs.c - main()

    Parámetros de entrada:
        + nbloques: El número de bloques que tiene el sistema de ficheros.
        + ninodos: El número de inodos que tiene el sistema de ficheros.

    Parámetros de salida:
        + 0: En ejecución correcta.
        + (-1): En caso contrario.
*/
int initSB(unsigned int nbloques, unsigned int ninodos){
    struct superbloque SB;

    //Posición del primer bloque del mapa de bits
    SB.posPrimerBloqueMB = posSB + 1;
    //Posición del último bloque del mapa de bits
	SB.posUltimoBloqueMB = SB.posPrimerBloqueMB + tamMB(nbloques) -1;
    //Posición del primer bloque del array de inodos
	SB.posPrimerBloqueAI = (SB.posUltimoBloqueMB +1);
    //Posición del último bloque del array de inodos
 	SB.posUltimoBloqueAI = (SB.posPrimerBloqueAI + tamAI(ninodos) -1);
    //Posición del primer bloque de datos
 	SB.posPrimerBloqueDatos = (SB.posUltimoBloqueAI +1);
    //Posición del último bloque de datos
 	SB.posUltimoBloqueDatos = (nbloques -1);
    //Posición del inodo del directorio raíz
 	SB.posInodoRaiz = 0;
    //Posición del primer inodo libre 
 	SB.posPrimerInodoLibre = 0;
    //Cantidad de bloques libres
    SB.cantBloquesLibres = nbloques;
 	/*nivel 3
     SB.cantBloquesLibres = (nbloques - tamMB(nbloques) - tamAI(ninodos) -1);
    */
    //Cantidad de inodos libres
 	SB.cantInodosLibres = ninodos;
    //Cantidad total de bloques 
 	SB.totBloques = nbloques;
    //Cantidad total de inodos
 	SB.totInodos = ninodos;

    //Escribir superbloque en a partir del bloque indicado
 	if (bwrite(posSB, &SB)==-1){
        //Check errores
		perror("Error de escritura en el dispositivo en ficheros_basicos.c\n");
        return -1;
	}

    return 0;
}

/*
    Descripción:
        Función que inicializa cada bit del mapa de bits del sistema de ficheros
        y escribe, byte a byte, cada sección inicializada del MB en el espacio de
        memoria, dentro del bloque correspondiente, reservado para ello.

    Funciones a las que llama:
        + bloques.h - bwrite()
        + bloques.h - bread()
    
    Funciones desde donde es llamado:
        + mi_mkfs.c - main()

    Parámetros de salida:
        + 0: En ejecución correcta.
        + (-1): En caso contrario.
*/
int initMB(){
    //Variables de apoyo
	int firstMB;
	int lastMB;

    //Buffer de apoyo inicializado
	unsigned char initCero[BLOCKSIZE];
	memset(initCero,0,BLOCKSIZE);

    //Estructura de apoyo
	struct superbloque SB;

    //Obtener información del superbloque
	if (bread(posSB, &SB) == -1) {
        perror("Error obteniendo información de superbloque al inicializar mapa de bits\n.");
        return -1;
    }

    //Obtener posiciones entre las cuales escribir
	firstMB = SB.posPrimerBloqueMB;
	lastMB = SB.posUltimoBloqueMB;
	
    //Escribir bloque a bloque dentro del sistema de ficheros
	for (int i = firstMB; i <= lastMB; i++){
	    if (bwrite(i,initCero) == -1) {
            perror("Error en inicialización de mapa de bits.");
            return -1;
        }
	}

    return 0;
}

/*
    Descripción:
        Función que inicializa el array de inodos y los entrelaza uno a uno.
        Escribe a partir del bloque del array de inodos del sistema de ficheros
        el array inicializado.
    
    Funciones a las que llama:
        + bloques.h - bwrite()
        + bloques.h - bread()

    Funciones desde donde es llamado:
        + mi_mkfs.c - main()

    Parámetros de salida:
        + 0: En ejecución correcta,
        + (-1): En caso contrario.
*/
int initAI() {
    //Estructuras y variables de apoyo
    struct superbloque SB;
    int contInodos;
    int numInPerBloq = BLOCKSIZE/INODOSIZE; //Número de inodos por bloque
    struct inodo inodos[numInPerBloq]; 

    if (bread(0, &SB) == -1) {
        perror("Error obteniendo información de superbloque al inicializar array de inodos\n.");
        return -1;
    }

    /*Pos. del inodo enlazado al primer inodo*/
    contInodos = SB.posPrimerInodoLibre + 1;

    //Inicialización de numInPerBloq inodos por cada bloque
    for(int i = SB.posPrimerBloqueAI; i <= SB.posUltimoBloqueAI; i++) {
        for(int j = 0; j < numInPerBloq; j++) {
            inodos[j].tipo = 'l';
           
            if (contInodos < SB.totInodos) {

                inodos[j].punterosDirectos[0] = contInodos; 
                contInodos++;

            } else {

                inodos[j].punterosDirectos[0] = UINT_MAX;
                break;

            }

        }

        //Escritura de datos en el bloque correspondiente
        if (bwrite(i, &inodos[0]) == -1) {
            perror("Error escribiendo datos de inodos en FS");
            return -1;
        }
    }
    return 0;
}

/*FUNCIONES DE NIVEL 3*/

/*
    Descripción: 
        Esta función escribe el valor indicado por el parámetro bit: 
        0 (libre) ó 1 (ocupado) en un determinado bit del MB que representa el bloque nbloque.
 
    Funciones a las que llama:
        + bread()
        + bwrite()

    Funciones desde donde es llamado: 

    Parámetros de entrada: 
        + unsigned int nbloque: 
        + unsigned int bit: 
    
    Parámetros de salida: 
        + 


 */
int escribir_bit(unsigned int nbloque, unsigned int bit) {

    //Declaraciones
    struct superbloque SB;
    int posbyte, posbit, nbloqueMB, nbloqueabs;
    char bytes[BLOCKSIZE];

    //Cálculo de valores.
    posbyte = nbloque/8; 
    posbit = nbloque % 8;
    nbloqueMB = posbyte/BLOCKSIZE;
    
    //Lectura del bloque correspondiente
    if (bread(posSB, &SB) == -1) {
        perror("Error: imposible leer superbloque en ficheros_basico.c - escribir_bit()");
        return -1;
    }

    //Posición absoluta del bloque en el cual escribir
    nbloqueabs = nbloqueMB + SB.posPrimerBloqueMB;

    if (bread(nbloqueabs, &bytes) == -1) {
        perror("Error: imposible leer información del MB en ficheros_basico.c - escribir_bit()");
        return -1;
    }
}

/*
    Descripción: 
        Lee un determinado bit del mapa de bits y devuelve el valor del bit
        leído.

        La función accede al byte correspondiente, dentro del bloque correspond-
        diente al que contiene el bit del bloque cuyo estado se quiere consultar
        dentro del mapa de bits del sistema de ficheros.

    Funciones a las que llama: 
        
    Parámetros de entrada: 
        + nbloque: El número de bloque del cual se desea consultar su estado

    Parámetros de salida: 
        +
*/
unsigned char leer_bit(unsigned int nbloque) {

    //Estructura de apoyo
    struct superbloque SB;

    if (bread(posSB, &SB) == -1) {
        printf("Error: no se ha podido obtener información del superbloque en leer_bit\n");
        return -1;
    }

    int posByte = nbloque/8;
    int posBit = nbloque%8;
    int nbloqueMB = posByte/BLOCKSIZE;
    int nbloqueMBabs = nbloque + SB.posPrimerBloqueMB;

    char bufferMB[BLOCKSIZE];

}

/*
    Descripción: 
        Encuentra el primer bloque libre consultando el mapa de bits, actualiza
        su estado dentro del mapa de bits y devuelve su posición.

    Funciones a las que llama: 
        + ficheros_basico.c - escribir_bit()

    Funciones desde donde es llamado:
        +

    Parámetros de salida: 
        +
*/
int reservar_bloque() {

}

/*
    Descripción: 
        Libera el bloque indicado por parámetro para su nueva reutilización.

    Funciones a las que llama: 
        + ficheros_basico.c - escribir_bit()

    Funciones desde donde es llamado:
        + 

    Parámetros de entrada: 
        + nbloque: El número de bloque del cual se desea liberar.

    Parámetros de salida: 
        + nbloque liberado. 
        + (-1) si se ha producido algún error durante el proceso. 
*/
int liberar_bloque(unsigned int nbloque) {

    //Declaraciones
    struct superbloque SB; 

    //Poner a 0 el bit del MB correspondiente al bloque nbloque. 
    if (escribir_bit(nbloque, 0)==-1) {

        printf("Error: no se ha podido liberar el bloque. Función -> liberar_bloque()\n");
        return -1; 

    }

    //Incrementar cantidad de bloques libres. 
    SB.cantBloquesLibres++;

    //Devolver el número de bloque liberado. 
    return nbloque; 
}

/*
    Descripción: 
        Escribe el contenido del inodo que recibe por paráḿetros dentro del
        inodo correspondiente en el array de inodos del sistema de ficheros.

    Funciones a las que llama: 
        + ficheros_basico.c - escribir_inodo()

    Parámetros de entrada: 
        + ninodo: El número de inodo donde escribir el contenido de inodo.
        + inodo: El inodo desde el cual copiar la información a escribir en 
                 inodo ninodo.

    Parámetros de salida: 
        +
*/
int escribir_inodo(unsigned int ninodo, struct inodo inodo) {

}

/*
    Descripción: 
        Lee un determinado inodo del array de inodos para volcarlo en una
        variable de tipo struct inodo pasada por referencia.

    Funciones a las que llama: 
        
    Parámetros de entrada: 
        + ninodo: El número de inodo a obtener.
        + inodo: El inodo al cual copiar la información a leer del inodo ninodo.

    Parámetros de salida: 
        +
*/
int leer_inodo(unsigned int ninodo, struct inodo *inodo){

}

/*
    Descripción: 
        Escribe el contenido del inodo que recibe por paráḿetros dentro del
        inodo correspondiente en el array de inodos del sistema de ficheros.

    Funciones a las que llama: 
        + 

        
    Parámetros de entrada: 
        + tipo: el tipo de inodo ('d': directorio | 'f': fichero | 'l': libre)
        + permisos: los permisos de lectura/escritura/ejecución:
                    ('r': lectura | 'w': escritura | 'x': ejecución).

    Parámetros de salida: 
        +
*/
int reservar_inodo(unsigned char tipo, unsigned char permisos) {
    //Declaraciones
    struct superbloque SB; 
    struct inodo i; 
    int posInodoReservado; 

    //Comprobar si hay inodos libres. 
    if (SB.cantInodosLibres<1) {
        printf("Error: No quedan inodos libres. Función -> reservar_inodo()\n");
        return -1; 
    }

    //Reservar el primer inodo libre. 
    posInodoReservado = SB.posPrimerInodoLibre;

    //Hacer que el superbloque apunte al siguiente inodo de la lista. 
    SB.posPrimerInodoLibre++;

    //Inicialización de los campos del inodo. 
    //Tipo
    i.tipo = tipo; 

    //Permisos
    i.permisos = permisos;

    //Cantidad de enlaces de entradas en directorio. 
    i.nlinks = 1; 

    //Tamaño en bytes lógicos.
    i.tamEnBytesLog = 0; 

    //timestamp de creación para todos los campos de fecha y hora. 
    i.atime = time(NULL);
    i.ctime = time(NULL); 
    i.mtime = time(NULL);
    
    //Cantidad de bloques ocupados en la zona de datos. 
    i.numBloquesOcupados = 0; 

    //Punteros a bloques directos. 
    i.punterosDirectos = 0; 

    //Punteros a bloques indirectos. 
    i.punterosIndirectos = 0;

    //Escribir inodo. 

    //Actualizar cantidad de inodos libres. 

    //Devolver posInodoReservado. 
    return posInodoReservado;
}

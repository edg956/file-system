#include "ficheros_basico.h"
#include <limits.h>
#include <time.h>

/*----------------------------FUNCIONES DE NIVEL 2----------------------------*/

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
		perror("Error: escritura en dispositivo incorrecta. Función -> initSB()");
        return -1;
	}

    return 0;
}

/*
    Descripción:
        Función que inicializa cada bit del mapa de bits del sistema de ficheros
        y escribe, byte a byte, cada sección inicializada del MB en el espacio 
        de memoria, dentro del bloque correspondiente, reservado para ello.

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
        perror("Error: no se ha podido leer del superbloque. Función -> initMB()");
        return -1;
    }

    //Obtener posiciones entre las cuales escribir
	firstMB = SB.posPrimerBloqueMB;
	lastMB = SB.posUltimoBloqueMB;
	
    //Escribir bloque a bloque dentro del sistema de ficheros
	for (int i = firstMB; i <= lastMB; i++){
	    if (bwrite(i,initCero) == -1) {
            perror("Error: inicialización de mapa de bits incorrecta. Función -> initMB()");
            return -1;
        }
	}

    //[Nivel 3] 
    //Modificación para tener en cuenta los bloques ocupados por los metadatos,
    //o sea el superbloque, el mapa de bits y el array de inodos. 
    for(int i = 0; i <= SB.posUltimoBloqueAI; i++) {
        
        if (escribir_bit(i,1) == -1) {
            perror("Error: asignación de bits ocupados del mapa de bits incorrecta. Función -> initMB()");
            return -1;
        }

    }
    
    //Restar bloques ocupados al total de bloques libres del superbloque.    
    SB.cantBloquesLibres = SB.cantBloquesLibres - SB.posUltimoBloqueAI;

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
        perror("Error: no se ha podido leer del superbloque. Función -> initAI()");
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
            perror("Error: Escritura de datos de inodos en FS incorrecta. Función -> initAI()");
            return -1;
        }
    }
    return 0;
}

/*----------------------------FUNCIONES DE NIVEL 3----------------------------*/

/*
    Descripción: 
        Esta función escribe el valor indicado por el parámetro bit: 
        0 (libre) ó 1 (ocupado) en un determinado bit del MB que representa 
        el bloque nbloque.
 
    Funciones a las que llama:
        + bloques.h - bread()
        + bloques.h - bwrite()

    Funciones desde donde es llamado:
        +

    Parámetros de entrada: 
        + unsigned int nbloque: 
        + unsigned int bit: 
    
    Parámetros de salida: 
        + 1: Ejecución correcta
        + (-1): Error

 */
int escribir_bit(unsigned int nbloque, unsigned int bit) {
    //Comprobación previa de errores. 
    if ((bit!=0) && (bit!=1)) {
        perror("Error: Un bit solo puede tener valor 1 o 0. Función -> escribir_bit()");
        return -1;
    }

    //Declaraciones
    struct superbloque SB;
    int posbyte, posbit, nbloqueMB, nbloqueabs;
    char bytes[BLOCKSIZE];
    int *bufferMB;
    unsigned char mascara = 128;

    //Cálculo de valores.
    posbyte = nbloque/8; 
    posbit = nbloque % 8;
    nbloqueMB = posbyte/BLOCKSIZE;
    
    //Lectura del bloque correspondiente
    if (bread(posSB, &SB) == -1) {
        perror("Error: no se ha podido leer del superbloque. Función -> escribir_bit()");
        return -1;
    }

    //Posición absoluta del bloque en el cual escribir
    nbloqueabs = nbloqueMB + SB.posPrimerBloqueMB;

    //Leemos el bloque que lo contiene y cargamos el contenido en el buffer. 
    *bufferMB = bread(nbloqueabs, &bytes); 

    //Comprobación de errores. 
    if (bufferMB == -1) {
        perror("Error: imposible leer información del MB. Función -> escribir_bit()");
        return -1;
    }

    posbyte = posbyte % BLOCKSIZE;

    //Desplazamiento de posbit bits a la derecha. 
    mascara >>= posbit; 

    //Poner a 0 o a 1 el bit. 
    if (bit == 0) {

        bufferMB[posbyte] &= ~mascara; 

    }else{

        bufferMB[posbyte] |= mascara;

    }

    //Escribir buffer del MB en el dispositivo virtual
    if (bwrite(nbloqueabs, bufferMB) == -1) {
        perror("Error: No se ha podido escribir en el array de inodos. Función -> escribir_bit()");
        return -1;
    }

    return 0;
}

/*
    Descripción: 
        Lee un determinado bit del mapa de bits y devuelve el valor del bit
        leído.

        La función accede al byte correspondiente, dentro del bloque correspond-
        diente al que contiene el bit del bloque cuyo estado se quiere consultar
        dentro del mapa de bits del sistema de ficheros.

    Funciones a las que llama: 
        + bloques.h - bread()

    Funciones desde donde es llamado:
        +
        
    Parámetros de entrada: 
        + nbloque: El número de bloque del cual se desea consultar su estado

    Parámetros de salida: 
        + $00: Si el bloque está libre
        + $01: Si el bloque está ocupado
        + $FF: Error
*/
unsigned char leer_bit(unsigned int nbloque) {

    //Estructura de apoyo
    struct superbloque SB;

    //Obtener información del superbloque
    if (bread(posSB, &SB) == -1) { //Check for errors
        perror("Error: no se ha podido leer del superbloque. Función -> leer_bit()");
        return -1;
    }

    //Variables de apoyo
    /*Se quiere calcular la posición del byte que contiene al bit que representa
    a nbloque dentro del conjunto de bytes del mapa de bits.*/
    int posByte = nbloque/8;    //Posición del byte que contiene el bit
    int posBit = nbloque%8;     //Posición del bit dentro de ese byte
 //   int nbloqueMB = posByte/BLOCKSIZE;  //Posición del bloque contiene el byte
    int nbloqueMBabs = nbloque + SB.posPrimerBloqueMB;  //Pos. Absoluta del bloq
   
    //Buffer de apoyo
    char bufferMB[BLOCKSIZE];

    //Obtener el bloque que contiene el byte buscado
    if (bread(nbloqueMBabs, &bufferMB) == -1) {
        perror("Error: no se ha podido leer para el buffer de MB. Función -> leer_bit()");
        return -1;
    }

    //Preparación de mascara y variables para obtener el bit
    unsigned char mask = 128;    //10000000

    mask >>= posBit;        //Desplazamiento a la derecha
    mask &= bufferMB[posByte%BLOCKSIZE];            //AND con mascara
    mask >>= (7-posBit);    //Desplazamiento a la derecha

    /*El LSB de mask tiene el estado del bit buscado*/

    return mask;
}

/*
    Descripción: 
        Encuentra el primer bloque libre consultando el mapa de bits, actualiza
        su estado dentro del mapa de bits y devuelve su posición.

    Funciones a las que llama: 
        + ficheros_basico.h - escribir_bit()
        + bloques.h - bread()
        + bloques.h - bwrite()

    Funciones desde donde es llamado:
        +

    Parámetros de salida: 
        + int: El número de bloque reservado
        + int: (-1) en caso de error
*/
int reservar_bloque() {
    struct superbloque SB;
    unsigned char bufferAux[BLOCKSIZE];
    unsigned char bufferMB[BLOCKSIZE];

    //Miramos si hay error al llamar bread y no encontramos el bloque deseado
    if (bread(posSB,&SB) < 0) {
        perror("Error: no se ha podido leer del superbloque. Función -> reservar_bloque()");
        return -1;
    }
    //Miramos si quedan bloques libres
    if (SB.cantBloquesLibres <= 0) {
        perror("Error: no hay bloques libres. Función -> reservar_bloque()");
        return -1;
    }
    
    //Inicializamos a 1s el bufferAux
    memset(bufferAux,255,BLOCKSIZE);

    int posBloqueMB = SB.posPrimerBloqueMB;

    //Miramos si hay error al leer el mapa de bits
    if (bread(posBloqueMB,bufferMB) < 0) {
        perror("Error: no se ha podido leer del mapa de bits. Función -> reservar_bloque()");
        return -1;
    }
    
    /*Comparamos cada bloque leído del MB, bufferMB, 
    con ese buffer auxiliar inicializado a 1s*/
    while (memcmp(bufferAux,bufferMB,BLOCKSIZE) == 0){
        posBloqueMB++;
        //Comprobamos que no haya error al leer el mapa de bits
        if(bread(posBloqueMB,bufferMB) < 0){
            perror("Error: no se ha podido leer del mapa de bits para comparar. Función -> reservar_bloque()");
            return -1;
        }
    }

    unsigned char byte = 255;
    unsigned char mascara = 128; //10000000
    int posByte = 0, posBit = 0, end = 0;

    
    while (posByte < BLOCKSIZE && !end){
        //Miramos si hay bits a 0 en el byte
        if (byte < 255){
            //Operador AND para bits
            while (bufferMB[posByte] & mascara){
                posBit++;
                bufferMB[posByte] <<= 1; //desplazamiento de bits a la izquierda
            }
            end = 1;    
            /*como hemos de localizar el primer bit 
            dentro del byte que vale 0, una vez la encontramos ya.*/
        }else{
            posByte++;  //si el byte no contiene 0s, vamos al siguiente
        }
        
    }

    int nBloque = ((posBloqueMB - SB.posPrimerBloqueMB) * BLOCKSIZE + posByte) * 8 + posBit;

    //Comprobamos que no haya error al escribir bit
    if (escribir_bit(nBloque,1) < 0){
        perror("Error: no se ha podido escribir el bit correspondiente a nbloque. Función -> reservar_bloque()");
        return -1;
    }
    SB.cantBloquesLibres--;
    
    //Comprobamos si hay error en el bwrite
    if (bwrite(posSB,&SB) < 0){
        perror("Error: no se ha podido escribir al superbloque. Función -> reservar_bloque()");
        return -1;
    }

    return nBloque;
}

/*
    Descripción: 
        Libera el bloque indicado por parámetro para su nueva reutilización.

    Funciones a las que llama: 
        + ficheros_basico.h - escribir_bit()

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

    //Llenar superbloque
    if (bread(posSB, &SB) == -1) {
        perror("Error: no se ha podido leer del superbloque. Función -> liberar_bloque()");
        return -1;
    }

    //Poner a 0 el bit del MB correspondiente al bloque nbloque. 
    if (escribir_bit(nbloque, 0)==-1) {

        perror("Error: no se ha podido liberar el bloque. Función -> liberar_bloque()");
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
        + bloques.h - bread()
        + bloques.h - bwrite()

    Funciones desde donde es llamado:
        +

    Parámetros de entrada: 
        + ninodo: El número de inodo donde escribir el contenido de inodo.
        + inodo: El inodo desde el cual copiar la información a escribir en 
                 inodo ninodo.

    Parámetros de salida: 
        + 0: Ejecución correcta
        + (-1): Error
*/
int escribir_inodo(unsigned int ninodo, struct inodo inodo) {
    //Estructura de superbloque para obtener información del FS
    struct superbloque SB;

    //Leer información del superbloque
    if (bread(posSB, &SB) == -1) {
        perror("Error en lectura de superbloque. Función -> escribir_inodo()");
        return -1;
    }

    //Variables de apoyo
    unsigned int posInodo = ninodo / NUMINPRBLQ; //Nº de blqs que tiene el inodo
    struct inodo bufferIn[NUMINPRBLQ];   //Buffer de inodos

    //Leer información del array de inodos
    if (bread(posInodo + SB.posPrimerBloqueAI, &bufferIn) == -1) {
        perror("Error en lectura desde el array de inodos. Función -> escribir_inodo()");
        return -1;
    }

    //Nº de inodo sobre el que escribir
    bufferIn[ninodo % NUMINPRBLQ] = inodo;

    //Escritura sobre el array de inodos
    if (bwritte(posInodo + SB.posPrimerBloqueAI, &bufferIn) == -1) {
        perror("Error en escritura al array de inodos. Función -> escribir_inodo()");

    if (bwrite(posInodo + SB.posPrimerBloqueAI, &bufferIn) == -1) {
        perror("Error en escritura al array de inodos.");
        return -1;
    }

    return 0;

}

/*
    Descripción: 
        Lee un determinado inodo del array de inodos para volcarlo en una
        variable de tipo struct inodo pasada por referencia.

    Funciones a las que llama:
        + bloques.h - bread() 
        
    Funciones desde donde es llamado:
        +

    Parámetros de entrada: 
        + ninodo: El número de inodo a obtener.
        + inodo: El inodo al cual copiar la información a leer del inodo ninodo.

    Parámetros de salida: 
        + inodo: Contiene el inodo leído del disco (modifica puntero)
        + int: 0 en ejecución correcta (return)
        + int: (-1) en error (return)
*/
int leer_inodo(unsigned int ninodo, struct inodo *inodo){
    struct superbloque SB;
    //Leemos el superbloque para obtener la localización del array de inodos
    if (bread(posSB,&SB) == -1) {
        perror("Error en lectura de superbloque. Función -> leer_inodo()");
        return -1;
    }

    //Obtenemos el número de bloque del array de inodos que tiene el inodo solicitado
    int posInodo = ((ninodo*INODOSIZE)/BLOCKSIZE) + SB.posPrimerBloqueAI;

    //Empleamos un arrau de inodos, del tamaño de la cantidad de inodos que caben en un bloque,
    //como buffer de lectura del bloque que hemos de leer
    struct inodo inodos[BLOCKSIZE/INODOSIZE];

    //Encontramos la posición del inodo solicitado
    if (bread(posInodo,inodo) == -1) {
        perror("Error en lectura del bloque que contiene ninodo. Función -> leer_inodo()");
        return -1;
    }

    //No se si está bien la asignación?¿
    *inodo = inodos[ninodo%(BLOCKSIZE/INODOSIZE)];
	return 0;
}

/*
    Descripción: 
        Escribe el contenido del inodo que recibe por paráḿetros dentro del
        inodo correspondiente en el array de inodos del sistema de ficheros.

    Funciones a las que llama: 
        + bloques.h - bread()
        + bloques.h - bwrite()
        + ficheros_basico.h - escribir_inodo()

    Funciones desde donde es llamado:
        +

    Parámetros de entrada: 
        + tipo: el tipo de inodo ('d': directorio | 'f': fichero | 'l': libre)
        + permisos: los permisos de lectura/escritura/ejecución:
                    ('r': lectura | 'w': escritura | 'x': ejecución).

    Parámetros de salida: 
        + int: Posición dentro del array de inodos del inodo reservado
        + (-1): Error
*/
int reservar_inodo(unsigned char tipo, unsigned char permisos) {
    //Declaraciones
    struct superbloque SB;

    //Llenar superbloque
    if (bread(posSB, &SB) == -1) {
        perror("Error: no se ha podido leer del superbloque. Función -> reservar_inodo()");
        return -1;
    }

    //Comprobar si hay inodos libres. 
    if (SB.cantInodosLibres<1) {
        perror("Error: No quedan inodos libres. Función -> reservar_inodo()\n");
        return -1; 
    }

    //Variables de apoyo
    struct inodo i; 
    int posInodoReservado; 

    //Reservar el primer inodo libre. 
    posInodoReservado = SB.posPrimerInodoLibre;

    //Hacer que el superbloque apunte al siguiente inodo de la lista. 
    SB.posPrimerInodoLibre++;

    //Inicialización de los campos del inodo. 
    i.tipo = tipo;          //tipo
    i.permisos = permisos;  //permisos
    i.nlinks = 1;           //Cantidad de enlaces de entradas en directorio. 
    i.tamEnBytesLog = 0;    //Tamaño en bytes lógicos.

    //timestamp de creación para todos los campos de fecha y hora. 
    i.atime = time(NULL);
    i.ctime = time(NULL); 
    i.mtime = time(NULL);
    
    //Cantidad de bloques ocupados en la zona de datos. 
    i.numBloquesOcupados = 0; 

    //Punteros a bloques directos.
    int numPtrDirect = sizeof(i.punterosDirectos) / sizeof(i.punterosDirectos[0]);
    for (int j = 0; j < numPtrDirect; j++) {
        i.punterosDirectos[j] = 0;
    } 

    //Punteros a bloques indirectos. 
    numPtrDirect = sizeof(i.punterosIndirectos) / sizeof(i.punterosIndirectos[0]);
    for (int j = 0; j < numPtrDirect; j++) {
        i.punterosIndirectos[j] = 0;
    } 

    //Escribir inodo.
    if (escribir_inodo(posInodoReservado, i) == -1) {
        perror("Error: Escritura de inodo reservado en array de inodos fallida. Función -> reservar_inodo()");
        return -1;
    }

    //Actualizar cantidad de inodos libres.
    SB.cantInodosLibres--;
    if (bwrite(posSB, &SB) == -1) {
        perror("Error: Actualizar superbloque fallido. Función -> reservar_inodo()");
        return -1;
    }

    //Devolver posInodoReservado. 
    return posInodoReservado;
}

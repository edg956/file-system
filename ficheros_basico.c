#include "ficheros_basico.h"

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
    int size;
    if (((nbloques / 8) % BLOCKSIZE) != 0){
        size = ((nbloques / 8) / BLOCKSIZE) + 1; //El modulo es diferente de 0 y por lo tanto hemos de añadir
                                                 //un bloque adicional para los bytes (el resto de la división)
    }
    else{
        size = ((nbloques / 8) / BLOCKSIZE); //El modulo es igual a 0 por lo tanto el numero de bloques será exacto
    }
    printf("tamMB: %i\n", size);
    return size;
}
/*
    Descripción:
        Función que calcula el número de inodos en bloques que tendrá el
        sistema de ficheros.
        Lo hace mediante la fórmula: (Número de inodos / INODOSIZE) / BLOCKSIZE

    Funciones desde donde es llamado:
        + mi_mkfs.c - main()

    Parámetros de entrada:
        + ninodos: El número de inodos que tiene el sistema de ficheros.
    
    Parámetros de salida:
        + Número de bloques necesarios para el número de inodos del sistema.
*/
int tamAI(unsigned int ninodos){
    int size;
    if (((ninodos * INODOSIZE) % BLOCKSIZE) != 0){
        size = ninodos * INODOSIZE / BLOCKSIZE + 1; //El modulo es diferente de 0 y por lo tanto necesitamos
                                                    //añadir un bloque adicional para los bytes (resto de la división)
    }
    else{
        size = ninodos * INODOSIZE / BLOCKSIZE; //El modulo es igual a 0 por lo que el número de bloque es exacto.
    }
    return size;
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

    SB.posPrimerBloqueMB = posSB + 1; //Posición del primer bloque del mapa de bits
    //printf("Posicio primer BloqueMB: %i\n",SB.posPrimerBloqueMB);
	SB.posUltimoBloqueMB = SB.posPrimerBloqueMB + tamMB(nbloques) -1; //Posición del último bloque del mapa de bits 
	//printf("Posicio ultim BloqueMB: %i\n",SB.posUltimoBloqueMB);
	SB.posPrimerBloqueAI = (SB.posUltimoBloqueMB +1); //Posición del primer bloque del array de inodos 
 	SB.posUltimoBloqueAI = (SB.posUltimoBloqueMB + tamAI(ninodos)); //Posición del último bloque del array de inodos 
 	SB.posPrimerBloqueDatos = (SB.posUltimoBloqueAI +1); //Posición del primer bloque de datos 
 	SB.posUltimoBloqueDatos = (nbloques -1); //Posición del último bloque de datos 
 	SB.posInodoRaiz = 0; //Posición del inodo del directorio raíz 
 	SB.posPrimerInodoLibre = 0; //Posición del primer inodo libre 
 	SB.cantBloquesLibres = (nbloques - tamMB(nbloques) - tamAI(ninodos) -1); //Cantidad de bloques libres
 	SB.cantInodosLibres = ninodos; //Cantidad de inodos libres 
 	SB.totBloques = nbloques; //Cantidad total de bloques 
 	SB.totInodos = ninodos; //Cantidad total de inodos 
 	if (bwrite(0,&SB)==-1){
		printf("Error de escritura en el dispositivo en ficheros_basicos.c\n");
	}
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
    int i;
	int firstMB;
	int lastMB;
	unsigned char initCero[BLOCKSIZE];

	struct superbloque SB;

	memset(&SB,0,BLOCKSIZE);
	bread(posSB,&SB);

	firstMB = SB.posPrimerBloqueMB;

	lastMB = SB.posUltimoBloqueMB;
	
	memset(initCero,0,BLOCKSIZE);

	for (i = firstMB; i <= lastMB; i++){
	    bwrite(i,initCero);
	}
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
int initAI() {}
#include "ficheros.h"

/*----------------------------FUNCIONES DE NIVEL 6----------------------------*/
/*
    Descripción: 
        Escribe el contenido de un buffer de memoria, buf_original, en un 
        fichero/directorio (correspondiente al inodo pasado como argumento): 
        le indicamos la posición de escritura inicial, offset, con respecto al 
        inodo (en bytes lógicos) y el número de bytes, nbytes, que hay que 
        escribir.

    Funciones a las que llama:
        + ficheros_basico.h - leer_inodo()
        + ficheros_basico.h - traducir_bloque_inodo()
        + ficheros_basico.h - escribir_inodo()
        + bloques.h - bwrite()
        + bloques.h - bread()

    Parámetros de entrada:
        + unsigned int ninodo
        + void *buf_original
        + unsigned int offset
        + unsigned int nbytes

    Parámetros de salida:
        + Cantidad de bytes escritos. 
        + (-1): Algún error ocurrido. 
*/
int mi_write_f(unsigned int ninodo, const void *buf_original, unsigned int offset, unsigned int nbytes) {
    struct inodo in;
    
    //Comprobar que el Nº de bytes a escribir sea mayor que 0
    if (nbytes <= 0) return 0;

    //Lectura del inodo
    if (leer_inodo(ninodo, &in) < 0) {
        perror("Error: no se ha podido leer el inodo. "
        "Función -> mi_write_f()");
        return -1;
    }
    
    //Comprobación de permisos
    if((in.permisos & 2) != 2){
        fprintf(stderr, "Error: Permiso denegado de escritura.\n");
        return -1;                                         
    }   

    //Calculamos el primer bloque lógico donde hay que escribir
    int bloqueI = offset / BLOCKSIZE; 
    //Calculamos el último bloque lógico donde hay que escribir                      
    int bloqueF = (offset + nbytes - 1) / BLOCKSIZE;
    int bfisico, bytes = 0;
	unsigned char bufBloque[BLOCKSIZE];             
    //Desplazamiento en el bloque
	int desp1 = offset % BLOCKSIZE;                         
	
    //Semáforo en traducir_bloque inodo porque reserva bloques.
    mi_waitSem();
    bfisico = traducir_bloque_inodo(ninodo, bloqueI, 1);
    mi_signalSem();

    //Si el bloque inicial y el bloque final coinciden
    if(bloqueI == bloqueF) {     

		if(bfisico < 0) {
            //Obtenemos el bloque físico Error Traducir Bloque Inodo
            return -1;                                      
        }

		if(bread(bfisico, bufBloque) < 0){
            return -1;                                     
        }

		memcpy (bufBloque + desp1, buf_original, nbytes); 

		if(bwrite(bfisico, bufBloque) < 0){
            return -1;
        } 

        // Aumentamos el contador los bytes que hemos escrito
		bytes += nbytes;	                   

    } else {
    
        //Primer bloque
        if (bfisico < 0) {
            return -1;
        }
        
        if(bread(bfisico, bufBloque) < 0) return -3;        //Leemos el bloque correspondiente Error BREAD

        memcpy (bufBloque + desp1, buf_original, BLOCKSIZE - desp1);
        
        if(bwrite(bfisico, bufBloque) < 0) {
            return -1;                                      // Error en el Bwrite
        }

        bytes += BLOCKSIZE - desp1;
    
        //Bloques Intermedios
        for(int i = bloqueI + 1; i < bloqueF; i++) {
            //Semáforo en traducir_bloque inodo porque reserva bloques.
            mi_waitSem();
            if((bfisico = traducir_bloque_inodo(ninodo, i, 1)) < 0) {
                mi_signalSem();
                return -1;                                  
            }
            mi_signalSem();

            bytes += bwrite (bfisico, buf_original + (BLOCKSIZE - desp1) 
            + (i - bloqueI - 1) * BLOCKSIZE);
        }

        //Último Bloque
        int desp2 = (offset + nbytes - 1) % BLOCKSIZE;
        
        //Semáforo en traducir_bloque inodo porque reserva bloques.
        mi_waitSem();
        if ((bfisico = traducir_bloque_inodo(ninodo, bloqueF, 1)) < 0){
            mi_signalSem();
            return -1;
        } 
        mi_signalSem();

        if(bread(bfisico, bufBloque) < 0) {
            return -1; 
        }
        memcpy (bufBloque, buf_original + (nbytes - desp2 - 1), desp2 + 1);
        if (bwrite(bfisico, bufBloque) < 0) {                                     
            return -1;
        }

        bytes += desp2 + 1;
    }

    mi_waitSem();

    //Lectura del inodo
    if (leer_inodo(ninodo, &in) < 0) {
        perror("Error: no se ha podido leer el inodo."
        "Función -> mi_write_f()");
        mi_signalSem();
        return -1;
    }

    //Actualizar metadatos inodo
    if (in.tamEnBytesLog < offset + bytes) {
        //Si se ha pasado el tamaño del fichero antes de la nueva escritura
        in.tamEnBytesLog = offset + bytes;
        in.ctime = time(NULL);
    }
    in.mtime = time(NULL);

    //Escribir inodo actualizado 
    if (escribir_inodo(ninodo, in) < 0) {
        perror("Error: no se ha podido escribir inodo."
        "Función -> mi_write_f()");
        mi_signalSem();
        return -1;
    }

    mi_signalSem();

    return bytes;
}

/*
    Descripción: 
        Lee información de un fichero/directorio (correspondiente al nº de inodo
        pasado como argumento) y la almacena en un buffer de memoria.

        Se debe recibir *buf_original inicializado con 0's. 

    Funciones a las que llama:
        + ficheros_basico.h - leer_inodo()
        + ficheros_basico.h - traducir_bloque_inodo()
        + bloques.h - bread()

    Parámetros de entrada:
        + unsigned int ninodo
        + void *buf_original
        + unsigned int offset
        + unsigned int nbytes

    Parámetros de salida:
        + Cantidad de bloques leídos. 
        + (-1): Algún error ocurrido. 
*/
int mi_read_f(unsigned int ninodo, void *buf_original, unsigned int offset, unsigned int nbytes){

    //Comprobar que el Nº de bytes a leer sea mayor que 0
    if (nbytes <= 0) return 0;

    //Declaraciones
    int leidos = 0; 
    struct inodo inodo; 
    int desp1 = offset%BLOCKSIZE; 
    int PBL = offset/BLOCKSIZE; 
    unsigned char auxBuff[BLOCKSIZE];
    int bfisico; 
    
    //Mutex lock
    mi_waitSem();

    //Lectura del inodo con el que se trabajará. 
    if (leer_inodo(ninodo, &inodo) == -1) {
       fprintf(stderr, "Error: no se ha podido leer el inodo deseado."
       "Función -> mi_read_f()\n");
       mi_signalSem(); //Mutex unlock
       exit(-1);
    }

    //Finalización.
    inodo.atime = time(NULL);

    if (escribir_inodo(ninodo, inodo) == -1) {
        fprintf(stderr, "Error: no se ha podido escribir el inodo actualizado "
        "Función -> mi_read_f()\n");
        mi_signalSem(); //Mutex unlock
        exit(-1);
    }

    //Mutex unlock
    mi_signalSem();

    //Comprobación inicial de los permisos de lectura del inodo. 
    if ((inodo.permisos & 4)!=4) { 
       fprintf(stderr, "Error: no se ha podido leer el inodo deseado. " 
       "Permisos incorrectos. Función -> mi_read_f()\n");
       exit(-1);
    }

    //Verificar que la posición del offset sea válida. 
    if (offset>= inodo.tamEnBytesLog) {
        return leidos; 
    }

    if (offset + nbytes >= inodo.tamEnBytesLog) {
        //Leer los bytes desde el offset hasta EOF. 
        nbytes = inodo.tamEnBytesLog - offset;       
    }

    //Se calcula el desplazamiento del último bloque y el último bloque. 
    int desp2 = (offset+nbytes-1)%BLOCKSIZE; 
    int UBL = (offset+nbytes-1)/BLOCKSIZE;

    /*PRIMER BLOQUE A LEER (Posibilidad de que sea incompleto)*/

    bfisico = traducir_bloque_inodo(ninodo, PBL, 0);

    if(PBL == UBL) {    //Si el bloque inicial y el bloque final coinciden

		if(bfisico < 0) {

            leidos += nbytes;//Obtenemos el bloque físico 

        }else{

		    if(bread(bfisico, auxBuff) < 0){
                return -1;
            }
        
		    memcpy (buf_original, auxBuff + desp1, nbytes);

            //Aumentamos el contador los bytes que hemos escrito
		    leidos += nbytes; 
        }
    }else{

    if ((bfisico)==-1) {

        leidos = leidos + (BLOCKSIZE-desp1);

    }else{

        //Se lee el primer bloque (entero) y se almacena en el buffer. 
        if (bread(bfisico, auxBuff)==-1) {
            fprintf(stderr, "Error: no se ha podido leer el bloque deseado." 
            "Función -> mi_read_f()");
            exit(-1);  
        }
        
        //Se realiza la copia al buffer original.
        memcpy(buf_original, auxBuff+desp1, BLOCKSIZE-desp1);

        //Se actualiza la variable de leídos. 
        leidos = leidos + (BLOCKSIZE-desp1);
    }


    /*BLOQUES INTERMEDIOS (Bloques que seguro que serán completos)*/

    for(int i = PBL+1; i < UBL; i++) {

        //Identificar bloque físico correspondiente. 
        bfisico = traducir_bloque_inodo(ninodo, i, 0);

        if ((bfisico)==-1) {

            leidos = leidos + BLOCKSIZE;
        
        }else{

            /*Se copia el bloque físico correspondiente al bloque lógico en
            un buffer auxiliar.*/
            if (bread(bfisico, auxBuff)==-1) {
                fprintf(stderr, "Error: no se ha podido leer el bloque deseado." 
                "Función -> mi_read_f()");
                exit(-1);   
            }

            /*Una vez leído el bloque, se tiene que copiar al buffer 
            original*/

            /*La suma de leidos es para ir avanzando dentro del buffer de
            destino y no pisar la información que haya antes*/
            memcpy(buf_original+leidos,auxBuff, BLOCKSIZE);

            //Se actualiza la variable que indica los bloques leídos.
            leidos = leidos+BLOCKSIZE;
        }
    }

    /*ÚLTIMO BLOQUE A LEER (Posibilidad de que sea incompleto)*/

    //Identificar el bloque físico. 
    bfisico = traducir_bloque_inodo(ninodo, UBL, 0);

    if ((bfisico)==-1) {

        leidos = leidos + desp2 + 1;

    } else {
        //Se lee el bloque entero y se almacena en el buffer auxiliar. 
        if (bread(bfisico, auxBuff)==-1) {
            fprintf(stderr, "Error: no se ha podido leer el bloque deseado." 
            "Función -> mi_read_f()");
            exit(-1);  
        }

        //Realizar la copia al buffer original. 
        memcpy((buf_original+nbytes)-desp2-1, auxBuff, desp2+1);

        //Se actualiza la variable de bloques leídos.
        leidos = leidos + desp2 + 1; 
    }
    }

    return leidos; 
}

/*
    Descripción: 
    Devuelve la metainformación de un fichero/directorio (correspondiente al 
    nº de inodo pasado como argumento): tipo, permisos, cantidad de enlaces 
    de entradas en directorio, tamaño en bytes lógicos, timestamps y cantidad 
    de bloques ocupados en la zona de datos, es decir todos los campos menos 
    los punteros.

    Funciones a las que llama:
        + ficheros_basico.h - leer_inodo()

    Parámetros de entrada:
        + unsigned int ninodo
        + struct STAT *p_stat

    Parámetros de salida:
        + 0: sin errores. 
        + (-1): algún error ocurrido.
        + Actualiza la información de la estructura apuntada por el puntero 
          *p_stat.  

*/
int mi_stat_f(unsigned int ninodo, struct STAT *p_stat){

    //Declaraciones
    struct inodo inodo; 

    //Lectura del inodo.
    if (leer_inodo(ninodo, &inodo)==-1) {
            perror("Error: no se ha podido leer el inodo deseado. "
                  "Función -> mi_stat_f()");
            exit(-1);
    }

    //Copiar los campos. 
    p_stat->tipo=inodo.tipo; 
    p_stat->permisos=inodo.permisos; 
    p_stat->atime=inodo.atime; 
    p_stat->mtime=inodo.mtime; 
    p_stat->ctime=inodo.ctime; 
    p_stat->nlinks=inodo.nlinks;
    p_stat->tamEnBytesLog=inodo.tamEnBytesLog; 
    p_stat->numBloquesOcupados=inodo.numBloquesOcupados; 

    return 0; 
}

/*
    Descripción: 
    Cambia los permisos de un fichero/directorio (correspondiente al nº de inodo
    pasado como argumento) según indique el argumento.

    Funciones a las que llama:
        + ficheros_basico.h - leer_inodo()
        + ficheros_basico.h - escribir_inodo()

    Parámetros de entrada:
        + unsigned int ninodo
        + unsigned char permisos

    Parámetros de salida:
        + 0: si no hay errores. 
        + (-1): si hay algún error.
*/
int mi_chmod_f(unsigned int ninodo, unsigned char permisos){

    //Declaraciones
    struct inodo inodo; 

    //Mutex lock
    mi_waitSem();

    if (leer_inodo(ninodo, &inodo)==-1) {
        perror("Error: no se ha podido leer el inodo deseado. "
                  "Función -> mi_chmod_f()");
        mi_signalSem(); //Mutex unlock
        exit(-1);        
    }

    //Cambiar los permisos. 
    inodo.permisos=permisos; 

    //Actualizar ctime. 
    inodo.ctime=time(NULL);

    //Escribir inodo ya modificado. 
    if (escribir_inodo(ninodo, inodo)==-1) {
        perror("Error: no se ha podido escribir el inodo deseado. "
                  "Función -> mi_chmod_f()");
        mi_signalSem(); //Mutex unlock
        exit(-1);
    }

    //Mutex unlock
    mi_signalSem();
    return 0;
}

/*
    Descripción: 
    Trunca un fichero/directorio (correspondiente al nº de inodo pasado como 
    argumento) a los bytes indicados, liberando los bloques necesarios.

    Funciones a las que llama:
        + ficheros_basico.h - leer_inodo()
        + ficheros_basico.h - escribir_inodo()
        + ficheros_basico.h - liberar_bloques_inodo()

    Parámetros de entrada:
        + unsigned int ninodo
        + unsigned int nbytes

    Parámetros de salida:
        + Número de bloques liberados. 
        + (-1): algún error ocurrido.
*/
int mi_truncar_f(unsigned int ninodo, unsigned int nbytes){

    //Declaraciones.
    struct inodo inodo; 
    int bliberados = 0; 
    int nblogico;

    //Leer inodo. 
    if (leer_inodo(ninodo, &inodo)==-1) {
            perror("Error: no se ha podido leer el inodo deseado. "
                  "Función -> mi_truncar_f()");
            exit(-1);  
    }

    //Comprobar que tiene permisos de escritura. 
    if ((inodo.permisos & 2) != 2) {
        perror("Error: no se ha podido truncar el fichero/directorio por"
        "falta de permisos de escritura. "
              "Función -> mi_truncar_f()");
                  
        return bliberados;         
    }

    //Comprobar que no se quiera truncar más allá del tamaño en bytes lógico.
    if (nbytes > inodo.tamEnBytesLog) {
        perror("Error: no se puede truncar el fichero/directorio más allá"
        " del tamaño en bytes lógico: "
              "Función -> mi_truncar_f()");
        return bliberados; 
    }

    //Determinar nblogico. 
    if ((nbytes % BLOCKSIZE)==0) {
        
        nblogico = nbytes/BLOCKSIZE; 

    }else{

        nblogico = nbytes/BLOCKSIZE+1;

    }

    bliberados = liberar_bloques_inodo(ninodo, nblogico);

    if (bliberados < 0) {
        fprintf(stderr, "Error: no se ha podido liberar el inodo %i\n", ninodo);
        return -1;
    }
    
    if (leer_inodo(ninodo, &inodo)==-1) {
            perror("Error: no se ha podido leer el inodo deseado. "
                  "Función -> mi_truncar_f()");
            exit(-1);  
    }

    //Actualizar mtime, ctime y tamaño en bytes lógicos del inodo. 
    inodo.ctime = time(NULL);
    inodo.mtime = time(NULL);
    inodo.tamEnBytesLog = nbytes;
    inodo.numBloquesOcupados -= bliberados;

    //Escribir inodo modificado. 
    if (escribir_inodo(ninodo, inodo)==-1) {
            perror("Error: no se ha podido escribir el inodo deseado"
              "Función -> mi_truncar_f()");
              exit(-1);
    }
    
    return bliberados; 
}
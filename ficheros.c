#include "ficheros.h"

/*----------------------------FUNCIONES DE NIVEL 6----------------------------*/

int mi_write_f(unsigned int ninodo, const void *buf_original, unsigned int offset, unsigned int nbytes) {
    struct inodo inodo;
    struct inodo in = leer_inodo(ninodo, &in);
    
    if((in.permisos & 2) != 2){
        return -1;                                          //Error no tiene permisos de escritura
    }   

    int bloqueI = offset / BLOCKSIZE;                       //Calculamos el primer bloque lógico donde hay que escribir
    int bloqueF = (offset + nbytes - 1) / BLOCKSIZE;        //Calculamos el último bloque lógico donde hay que escribir
    int bfisico, bytes = 0;
	unsigned char bufBloque[BLOCKSIZE];             
	int desp1 = offset % BLOCKSIZE;                         //Desplazamiento en el bloque
	
    if(bloqueI == bloqueF) {                                //Si el bloque inicial y el bloque final coinciden
		if(traducir_bloque_inodo(ninodo, bloqueI, 1) < 0){
            return -1;                                      //Obtenemos el bloque físico Error Traducir Bloque Inodo
        }
		if(bread(bfisico, bufBloque) < 0){
            return -1;                                      //Error en el Bread
        }
		memcpy (bufBloque + desp1, buf_original, BLOCKSIZE - desp1);

		if(bwrite(bfisico, bufBloque) < 0){
            return -1;                                      // Error en el Bwrite
        } 
		bytes += BLOCKSIZE - desp1 + 1;	                    // Aumentamos el contador los bytes que hemos escrito
    }else{
        //Primer bloque
        if(traducir_bloque_inodo(ninodo, bloqueI,1) < 0) {
            return -1;                                      //Error Traducir Bloque Inodo
        }
        if(bread(bfisico, bufBloque) < 0) return -3;        //Leemos el bloque correspondiente Error BREAD
        memcpy (bufBloque + desp1, buf_original, BLOCKSIZE - desp1);
        if(bwrite(bfisico, bufBloque) < 0) {
            return -1;                                      // Error en el Bwrite
        }
        bytes += BLOCKSIZE - desp1;
    
        //Bloques Intermedios
        for(int i = bloqueI + 1; i < bloqueF; i++) {
            if(traducir_bloque_inodo(ninodo, i, 1) < 0) {
                return -1;                                  //Error Traducir Bloque Inodo
            }
            bytes += bwrite (bfisico, buf_original + (BLOCKSIZE - desp1) + (i - bloqueI - 1) * BLOCKSIZE);
        }
        //Último Bloque
        int desp2 = (offset + nbytes - 1) % BLOCKSIZE;
        if (traducir_bloque_inodo(ninodo, bloqueF, 1) < 0){
            return -1;                                      //Error Traducir Bloque Indodo

        } 
        if(bread(bfisico, bufBloque) < 0) {
            return -1;                                      //Error en el Bread
        }
        memcpy (bufBloque, buf_original + (nbytes - desp2 - 1), desp2 + 1);
        if (bwrite(bfisico, bufBloque) < 0) {
            return -1;                                       // Error en el Bwrite
        }
        bytes += desp2 + 1;
    }
    //falta actualizar la metainformación del inodo
}

int mi_read_f(unsigned int ninodo, void *buf_original, unsigned int offset, unsigned int nbytes){

    //Declaraciones
    int leidos = 0; 
    struct inodo inodo; 
    int desp1 = offset%BLOCKSIZE; 
    int desp2 = (offset+nbytes-1)%BLOCKSIZE; 
    int PBL = offset/BLOCKSIZE; 
    int UBL = (offset+nbytes-1)/BLOCKSIZE;
    unsigned char *auxBuff[BLOCKSIZE];
    int bfisico; 
    
    //Lectura del inodo con el que se trabajará. 
    if (leer_inodo(ninodo, &inodo) == -1) {
       perror("Error: no se ha podido leer el inodo deseado.");
       exit(-1);
    }

    //Comprobación inicial de los permisos de lectura del inodo. 
    if ((inodo.permisos & 4)==4) {

        //Verificar que la posición del offset sea válida. 
        if (offset>= inodo.tamEnBytesLog) {
            return leidos; 
        }

        /*Si no entiendo mal, en la documentación pone que este pedazo de código tiene
        que ir justo aqui, pero el código se va a extender mucho y me parece que tiene
        que haber alguna forma mucho más eficiente de hacerlo. */

        if (offset + nbytes >= inodo.tamEnBytesLog) {
            nbytes = inodo.tamEnBytesLog - offset;
            //Leer los bytes desde el offset hasta EOF. 


        }

        for(int i = PBL; i <= UBL; i++) {
            
            //Identificación del bloque físico correspondiente. 
            bfisico = traducir_bloque_inodo(ninodo, i, 0);

            if (bfisico==-1) {
                
                /*Bloque físico inexistente. Se acumulan los bytes leídos
                y se sigue iterando*/
                leidos = leidos+BLOCKSIZE;

            }else{
                
              //Se deposita el contenido del bloque en el buffer auxiliar.  
              if (bread(bfisico, *auxBuff)==-1) {
                  perror("Error: no se ha podido leer el bloque deseado. "
                  "Función -> mi_read_f()");
                  exit(-1);
              }
               
               if (i==PBL) {

                   //Primer bloque (no completo).
                  memcpy(buf_original, auxBuff+desp1, BLOCKSIZE-desp1);
                  leidos = leidos+BLOCKSIZE-desp1;

              }else if (i==UBL) {

                    //Úlimo bloque (no completo).
                    memcpy(buf_original+nbytes-desp2, auxBuff, desp2+1);
                    leidos = leidos+desp2+1; 

                }else{

                    //Bloques de por el medio (completos). 
                    memcpy(buf_original+leidos, auxBuff, BLOCKSIZE);
                    leidos = leidos+BLOCKSIZE;

                }
            }
        
        }
        //Finalización.
        inodo.atime = time(NULL);

    }

    return leidos; 

}

int mi_stat_f(unsigned int ninodo, struct STAT *p_stat){

}

int mi_chmod_f(unsigned int ninodo, unsigned char permisos){

}

int mi_truncar_f(unsigned int ninodo, unsigned int nbytes){

}
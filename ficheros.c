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

}

int mi_stat_f(unsigned int ninodo, struct STAT *p_stat){

}

int mi_chmod_f(unsigned int ninodo, unsigned char permisos){

}

int mi_truncar_f(unsigned int ninodo, unsigned int nbytes){

}
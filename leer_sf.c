/*
    PROGRAMA DE APOYO PARA DETERMINAR LA CORRECIÓN DE LA INFORMACIÓN
    ALMACENADA EN EL SUPERBLOQUE, EN EL MAPA DE BITS Y EN EL ARRAY DE INODOS
*/
#include <time.h>
#include "bloques.h"
#include "ficheros_basico.h"

#define NUM_ATTR_SB 6 //Número de atributos sobre los cuales iterar para 
                      //función leer_bit()
#define POS_INODO_RAIZ 0 //Posición del inodo raiz dentro del array de inodos

int main(int argc, char **argv) {

    //Comprobación de parámetros enviados al programa. 
    if (argc != 2) {
        perror("Error: formato de comando: ./nombre_de_programa <nombre_de_fichero_disco>\n");
        exit(-1);
    }

    //Montaje del disco virtual. 
    if (bmount(argv[1]) == -1) {
        perror("Error: no se ha podido abrir el directorio indicado.\n");
        exit(-1);
    }

    //Estructuras y variables de apoyo.
    struct superbloque SB;

    //Lectura del superbloque.
    if (bread(posSB, &SB) == -1) {
        perror("Error: no se ha podido obtener información del superbloque del FS\n");
        exit(-1);
    }

    /**************************DATOS DEL SUPERBLOQUE***************************/
    puts("\nDATOS DEL SUPERBLOQUE");
    printf("Posición del primer bloque del mapa de bits: %i\n",SB.posPrimerBloqueMB);
    printf("Posición del último bloque del mapa de bits: %i\n",SB.posUltimoBloqueMB);
    printf("Posición del primer bloque del array de inodos: %i\n",SB.posPrimerBloqueAI);
    printf("Posición del último bloque del array de inodos: %i\n",SB.posUltimoBloqueAI);
    printf("Posición del primer bloque de datos: %i\n",SB.posPrimerBloqueDatos);
    printf("Posición del último bloque de datos: %i\n",SB.posUltimoBloqueDatos);
    printf("Posición del inodo del directorio raiz: %i\n", SB.posInodoRaiz);
    printf("Posición del primer inodo libre: %i\n",SB.posPrimerInodoLibre);
    printf("Cantidad de bloques libres: %i\n",SB.cantBloquesLibres);
    printf("Cantidad de inodos libres: %i\n",SB.cantInodosLibres);
    printf("Cantidad total de bloques: %i\n",SB.totBloques);
    printf("Cantidad total de inodos: %i\n",SB.totInodos);

    //Desmontaje del dispositivo virtual. 
    if (bumount() == -1) {
        perror("Error: desmontar dispositivo erroneo.");
        exit(-1);
    }
<<<<<<< HEAD
    
=======
>>>>>>> Código compilable. He estado intentando hacer funcionar el script 1. De momento no ha sido posible.
}
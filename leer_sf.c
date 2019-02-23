/*
    PROGRAMA DE APOYO PARA DETERMINAR LA CORRECIÓN DE LA INFORMACIÓN
    ALMACENADA EN EL SUPERBLOQUE, EN EL MAPA DE BITS Y EN EL ARRAY DE INODOS
*/

#include "bloques.h"
#include "ficheros_basico.h"

int main(int argc, char **argv) {

    if (argc != 2) {
        printf("Error: número de argumentos excedido.\n");
        exit(-1);
    }

    if (bmount(argv[1]) == -1) {
        printf("Error: no se ha podido abrir el directorio indicado.\n");
        exit(-1);
    }

    if (bmount(argv[1]) == -1) {
        printf("Error: no se ha podido abrir la ruta especificada.\n");
        exit(-1);
    }

    //Estructuras y variables de apoyo
    struct superbloque SB;

    if (bread(posSB, &SB) == -1) {
        printf("Error: no se ha podido obtener información del superbloque del FS\n");
        exit(-1);
    }

    printf("DATOS DEL SUPERBLOQUE\n");
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

    printf("sizeof struct superbloque: %li\n",sizeof(struct superbloque));
    printf("sizeof struct inodo: %li\n",sizeof(struct inodo));

    printf("RECORRIDO LISTA ENLAZADA DE INODOS LIBRES:\n");
    
    struct inodo inodos[SB.totInodos];

    if (bread(SB.posPrimerBloqueAI, &inodos) == -1) {
        printf("Error: no se ha podido obtener información del array de inodos del FS\n");
        exit(-1);
    }

    for (int i = 0; i < SB.totInodos; i++) {
        printf("%i ",inodos[i].punterosDirectos[0]);
    }

    if (bumount() == -1) {
        printf("Error: no se ha podido cerrar el fichero.\n");
        exit(-1);
    }



}
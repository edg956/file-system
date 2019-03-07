/*
    PROGRAMA DE APOYO PARA DETERMINAR LA CORRECIÓN DE LA INFORMACIÓN
    ALMACENADA EN EL SUPERBLOQUE, EN EL MAPA DE BITS Y EN EL ARRAY DE INODOS
*/

#include "bloques.h"
#include "ficheros_basico.h"

int main(int argc, char **argv) {

    if (argc != 2) {
        perror("Error: formato de comando: ./nombre_de_programa <nombre_de_fichero_disco>\n");
        exit(-1);
    }

    if (bmount(argv[1]) == -1) {
        perror("Error: no se ha podido abrir el directorio indicado.\n");
        exit(-1);
    }

    //Estructuras y variables de apoyo


    struct superbloque SB;


    if (bread(posSB, &SB) == -1) {
        perror("Error: no se ha podido obtener información del superbloque del FS\n");
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
    
 /*  printf("\nMOSTRAR MB\n");
    for(int i = 0; i < 100000; i++) {
        
        printf("%d, ",  leer_bit(i));

    }*/
    


    printf("\nRESERVAMOS UN BLOQUE Y LUEGO LO LIBERAMOS\n");

    int rbloque = reservar_bloque();
    printf("Bloque reservado: %d\n", rbloque);


    if (bread(posSB, &SB) == -1) {
        perror("Error: no se ha podido obtener información del superbloque del FS\n");
        exit(-1);
    }

    
    printf("Cantidad de bloques libres: %i\n",SB.cantBloquesLibres);
    printf("Liberación\n");

    liberar_bloque(rbloque);
 if (bread(posSB, &SB) == -1) {
        perror("Error: no se ha podido obtener información del superbloque del FS\n");
        exit(-1);
    }

    
    printf("Cantidad de bloques libres: %i\n",SB.cantBloquesLibres);
    //Lectura del array de inodos, obteniendolo bloque por bloque
 /*   for (int i = SB.posPrimerBloqueAI; i <= SB.posUltimoBloqueAI; i++) {
        if (bread(i, &inodos[0]) == -1) {
            perror("Error: no se ha podido obtener información del array de inodos del FS\n");
            exit(-1);
        }

        for (int i = 0; i < BLOCKSIZE/INODOSIZE; i++) {
            printf("%i ",inodos[i].punterosDirectos[0]);

        }
    }
    
    puts("");*/

    if (bumount() == -1) {
        perror("Error: no se ha podido cerrar el fichero.\n");
        exit(-1);
    }
}
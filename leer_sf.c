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

    //Estructuras y variables de apoyo
    struct superbloque SB;

    if (bread(0,&SB) == -1) {
        printf("Error: no se ha podido obtener información del superbloque del FS\n");
        exit(-1);
    }

    printf("DATOS DEL SUPERBLOQUE\n");
    printf("posPrimerBloqueMB = %d",1);
}
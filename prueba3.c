#include "bloques.h"
#include "directorios.h"

int main(int argc, char **argv) {

    //Montaje del disco virtual. 
    if (bmount(argv[1]) == -1) {
        fprintf(stderr, "Error: no se ha podido abrir el directorio indicado.\n");
        exit(-1);
    }

    struct STAT stat; 

    for (int i = 0; i < 10; i++) {

        mi_stat_f(i, &stat);
        printf("\n\nNúmero: %d\n", i);
        printf("Tipo: %c\n", stat.tipo);
        printf("TamEnBytesLog: %d\n", stat.tamEnBytesLog);
        printf("Permisos: %d\n", stat.permisos); 

    }

    //Desmontaje del dispositivo virtual. 
    if (bumount() == -1) {
        fprintf(stderr, "Error: no se ha podido cerrar el fichero.\n");
        exit(-1);
    }
}
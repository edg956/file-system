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

    if (argc != 3) {
        perror("Error: formato de comando: ./nombre_de_programa <nombre_de_fichero_disco> <Nº Inodo>\n");
        exit(-1);
    }

    if (bmount(argv[1]) == -1) {
        perror("Error: no se ha podido abrir el directorio indicado.\n");
        exit(-1);
    }

    /******************INODO 1: LIBERACIÓN DE BLOQUES LÓGICOS******************/

    int ninodo = atoi(argv[2]);

    printf("\nLIBERACIÓN DE INODO: %i\n", ninodo);

    clock_t str_time,fns_time;
    double segundos;
    str_time = clock();
    
    liberar_inodo(ninodo);
    
    fns_time = clock();

    segundos = (double) (fns_time-str_time) / CLOCKS_PER_SEC;

    printf("Tiempo de liberación de inodo %i: %f\n", ninodo,segundos);

    if (bumount() == -1) {
        perror("Error: no se ha podido cerrar el fichero.\n");
        exit(-1);
    }

}
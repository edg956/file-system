/*
    PROGRAMA PARA BORRAR UN FICHERO O DIRECTORIO. 
*/

#include "bloques.h"
#include "directorios.h"
#include <string.h>

int main(int argc, char **argv) {

    //Declaraciones
    char errbuff[BLOCKSIZE];
    char answer = 'n';
    int nerror;

    //Inicialización del buffer de errores. 
    memset(errbuff, 0, sizeof(errbuff));

    //Comprobación de parámetros enviados al programa. 
    if (argc != 3 && argc != 4) {
        fprintf(stderr, "Error: Sintaxis: /mi_rm [<flag -r>] <disco> </ruta>\n");
        exit(-1);
    }

    //Comprobar que no se intente eliminar la raíz. 
    if (!strcmp(argv[argc-1],"/")) {
        fprintf(stderr, "Error: No se puede eliminar el directorio raíz.\n");
        exit(-1);
    }

    /*
        Si se llama a rm con el flag -r, verificar sintaxis correcta
    */
    if (argc == 4) {
        if (strlen(argv[1]) != 2) {
            fprintf(stderr, "Error: Nº de flags sobre el límite. Intentar -r\n");
            exit(-1);
        }

        if (argv[1][0] == '-') {
            if (argv[1][1] != 'r') {
                fprintf(stderr, "Error: flag %c desconocido. Intentar -r.\n",
                    argv[1][1]);
                exit(-1);
            }
            
            //Comprobar con usuario eliminar de manera recursiva
            printf("Se va a proceder a eliminar todos los ficheros y "
            "subdirectorios de %s. (Y/n): ", argv[3]);
            scanf("%c", &answer);

            //Check entrada
            if (answer != 'Y' && answer != 'y') {
                puts("Ejecución cancelada.");
                exit(-1);
            }

        } else {
            fprintf(stderr, "Error: parámetro %c desconocido. ¿Quiso decir -r?\n",
                argv[1][0]);
            exit(-1);
        }
    }

    //Montaje del disco virtual. 
    if (bmount(argv[argc-2]) == -1) {
        fprintf(stderr, "Error: no se ha podido abrir el directorio indicado.\n");
        exit(-1);
    }

    //Llamada a la función mi_unlink o mi_unlink_r. 
    if (answer == 'Y' || answer == 'y') {
        nerror = mi_unlink_r(argv[argc-1]);
    } else {
        nerror = mi_unlink(argv[argc-1]); 
    }

    //Si ha habido error, visualizar mensaje correspondiente.
    if (nerror < 0) {
        control_errores_buscar_entrada(nerror, errbuff);
        fprintf(stderr, "%s", errbuff);
        exit(-1);
    }
    
    //Desmontaje del dispositivo virtual. 
    if (bumount() == -1) {
        fprintf(stderr, "Error: no se ha podido cerrar el fichero.\n");
        exit(-1);
    }
}
//FICHERO DE VERIFICACIÓN 
#include "verificacion.h"

int main(int argc, char **argv) {

    //Declaraciones
    struct STAT stat; 
    int nentradas; 
    char buffentradas[NPROC*sizeof(struct entrada)];
    char dirsimulacion[128]; 

    //Comprobar el número de argumentos. 
    if (argc!=3) {
        fprintf(stderr, "Error: Sintaxis: ./nombre_de_programa <disco> <directorio_simulacion>\n");
        exit(-1);
    }

    //Copiar el directorio de simulación. 
    strcpy(dirsimulacion, argv[2]);

    //Montar disco. 
    if (bmount(argv[1])==-1) {
        fprintf(stderr, "Error: El disco no se ha podido montar.\n");
        exit(-1);
    }

    //Realización del STAT. 
    if (mi_stat(argv[2], &stat)==-1) {
        fprintf(stderr, "Error: El disco no se ha podido montar.\n");
        exit(-1);
    }

    //Calcular número de entradas del directorio. 
    nentradas = stat.tamEnBytesLog/sizeof(struct entrada);

    if (nentradas != NPROC) {
        fprintf(stderr, "Error: El número de entradas es diferente al número de procesos.");
        exit(-1);
    }

    //Concatenar el nombre del informe a la ruta de simulación. 
    strcat(dirsimulacion, "informe.txt");

    //Crear el fichero informe.txt dentro del directorio de simulación.
    if (mi_touch(dirsimulacion, 6)==-1) {
        fprintf(stderr, "Error: No se ha podido crear el informe.\n");
        exit(-1);
    }

    //MEJORA
    //Lectura de todas las entradas entes de entrar al bucle para almacenarlas
    //en un buffer. 
    mi_read(argv[2], buffentradas, 0, stat.tamEnBytesLog);

    for (int i=0; i<nentradas; i++) {
        
        //Leer entrada del directorio. 
        //mi_read(argv[2], );

        //Extraer el PID a partir del nombre de la entrada y guardarlo en el registro info. 


        /* Recorrer secuencialmente el fichero prueba.dat utilizando un buffer 
        de N registros de escrituras*/
       // while()


    }


}
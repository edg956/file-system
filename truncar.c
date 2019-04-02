#include "ficheros.h"
#include "bloques.h"

int main(int argc, char **argv) {

    //Declaraciones. 
    int bliberados; 
    struct superbloque superbloque; 
    struct inodo inodo; 

    //Comprobar la sintaxis de la llamada a la función.
    if (argc!=4) {
        perror("Error: Sintaxis de llamada al programa incorrecta.\n"
        "Sintaxis: ./nombre_del_programa <nombre_dispositivo>" 
        "<ninodo> <nbytes\n>");
        exit(-1);
    }

    //Montar dispositivo virtual. 
    if (bmount(argv[1]) == -1) {
        perror("Error: no se ha podido abrir el directorio indicado.\n");
        exit(-1);
    }

    //Leer superbloque
    if (bread(posSB, &superbloque) < 0) {
        perror("Error: no se ha podido leer el superbloque."
        "Función -> main()");
        exit(-1);
    }

    //Leer inodo. 
    if (leer_inodo(atoi(argv[2]), &inodo)==-1) {
        perror("Error: no se ha podido leer el inodo indicado." 
        "Función -> truncar.c - main()\n");
        exit(-1);    
    }

    //Imprimir cantBloquesLibres y numBloquesOcupados del superbloque e inodo.  
    printf("Cantidad de bloques libres: %d\n", superbloque.cantBloquesLibres);
    printf("Número de bloques ocupados: %d\n", inodo.numBloquesOcupados);

    //Llamada a la función mi_truncar_f().
    bliberados = mi_truncar_f(atoi(argv[1]), atoi(argv[2]));

    if (bliberados==-1) {
        perror("Error: no se ha podido abrir el directorio indicado.\n");
        exit(-1);
    }

    //Mostrar número de bloques liberados.
    printf("Número de bloques liberados: %d\n", bliberados);

    //Imprimir cantBloquesLibres y numBloquesOcupados del superbloque e inodo.  
    printf("Cantidad de bloques libres: %d\n", superbloque.cantBloquesLibres);
    printf("Número de bloques ocupados: %d\n", inodo.numBloquesOcupados);
    
    //Desmontar dispositivo virtual. 
    if (bumount()==-1) {
        perror("Error: no se ha podido desmontar el dispositivo virtual.\n");
        exit(-1);
    }

}

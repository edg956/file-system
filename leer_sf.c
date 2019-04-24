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

    /**************************DATOS DEL SUPERBLOQUE***************************/

    puts("DATOS DEL SUPERBLOQUE\n");
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
    
    /*****************************RESERVAR BLOQUE******************************/

    
    /************************DATOS DEL DIRECTORIO RAIZ*************************/

    printf("DATOS DEL DIRECTORIO RAIZ: \n\n");

    struct tm *ts;
    char atime[80];
    char mtime[80];
    char c_time[80];
    struct inodo inodo;
    unsigned int ninodo = POS_INODO_RAIZ;

    if (leer_inodo(ninodo, &inodo) == -1) {
        perror("Error leyendo el inodo raiz");
        exit(-1);
    }

    ts = localtime(&inodo.atime);
    strftime(atime, sizeof(atime), "%a %Y-%m-%d %H:%M:%S",ts);
    ts = localtime(&inodo.mtime);
    strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S",ts);
    ts = localtime(&inodo.ctime);
    strftime(c_time, sizeof(c_time), "%a %Y-%m-%d %H:%M:%S", ts);
    
    printf("tipo: %c\n",inodo.tipo);
    printf("Permisos: %i\n",inodo.permisos);
    printf("atime: %s\n",atime);
    printf("ctime: %s\n", c_time);
    printf("mtime: %s\n",mtime);
    printf("nlinks: %i\n",inodo.nlinks);
    printf("tamEnBytesLog: %i\n",inodo.tamEnBytesLog);
    printf("numBloquesOcupados: %i\n",inodo.numBloquesOcupados);

    /******************INODO 1: TRADUCCIÓN DE BLOQUES LÓGICOS******************/

    unsigned int blogicos[5] = {8,204,30004,400004,16843019};
    int sizearray = sizeof(blogicos) / sizeof(blogicos[0]);
    
    //Obtener primer inodo libre
    if ((ninodo = reservar_inodo(T_INODO_FICHERO, 6)) == -1) {
        perror("Error en reservar inodo.");
        exit(-1);
    }

    printf("\nINODO %i. TRADUCCIÓN DE BLOQUES LÓGICOS 8, 204, 30.004, 400.004"
    " y 16.843.019\n\n",ninodo);

    
    //Asignar punteros a inodo
    for (int i = 0; i < sizearray; i++) {
        traducir_bloque_inodo(ninodo, blogicos[i],1);
    }
    
    /************************DATOS DEL INOODO RESERVADO************************/

    //Lectura del nuevo inodo
    if (leer_inodo(ninodo, &inodo) == -1) {
        perror("Error: no se ha podido leer el inodo deseado.");
        exit(-1);
    }


    //Formatear tiempos
    ts = localtime(&inodo.atime);
    strftime(atime, sizeof(atime), "%a %Y-%m-%d %H:%M:%S",ts);
    ts = localtime(&inodo.mtime);
    strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S",ts);
    ts = localtime(&inodo.ctime);
    strftime(c_time, sizeof(c_time), "%a %Y-%m-%d %H:%M:%S", ts);

    printf("\nDATOS DEL INODO RESERVADO %i\n",ninodo);
    printf("tipo: %c\n",inodo.tipo);
    printf("Permisos: %i\n",inodo.permisos);
    printf("atime: %s\n",atime);
    printf("ctime: %s\n", c_time);
    printf("mtime: %s\n",mtime);
    printf("nlinks: %i\n",inodo.nlinks);
    printf("tamEnBytesLog: %i\n",inodo.tamEnBytesLog);
    printf("numBloquesOcupados: %i\n",inodo.numBloquesOcupados);



    /******************INODO 1: LIBERACIÓN DE BLOQUES LÓGICOS******************/

    printf("\nLIBERACIÓN DE INODO %i\n:", ninodo);

    

    clock_t str_time,fns_time;
    double segundos;
    str_time = clock();
    
    liberar_inodo(ninodo);
    

    fns_time = clock();

    segundos = (double) (fns_time-str_time) / CLOCKS_PER_SEC;

    printf("Tiempo de liberación de inodo %i: %f\n", ninodo,segundos);

     //Lectura del nuevo inodo
    if (leer_inodo(ninodo, &inodo) == -1) {
        perror("Error: no se ha podido leer el inodo deseado.");
        exit(-1);
    }


    //Formatear tiempos
    ts = localtime(&inodo.atime);
    strftime(atime, sizeof(atime), "%a %Y-%m-%d %H:%M:%S",ts);
    ts = localtime(&inodo.mtime);
    strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S",ts);
    ts = localtime(&inodo.ctime);
    strftime(c_time, sizeof(c_time), "%a %Y-%m-%d %H:%M:%S", ts);

    printf("\nDATOS DEL INODO RESERVADO %i\n",ninodo);
    printf("tipo: %c\n",inodo.tipo);
    printf("Permisos: %i\n",inodo.permisos);
    printf("atime: %s\n",atime);
    printf("ctime: %s\n", c_time);
    printf("mtime: %s\n",mtime);
    printf("nlinks: %i\n",inodo.nlinks);
    printf("tamEnBytesLog: %i\n",inodo.tamEnBytesLog);
    printf("numBloquesOcupados: %i\n",inodo.numBloquesOcupados);


    if (bumount() == -1) {
        perror("Error: no se ha podido cerrar el fichero.\n");
        exit(-1);
    }

}
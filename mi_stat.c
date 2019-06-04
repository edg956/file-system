/*
    PROGRAMA PARA CONSULTAR LOS METADATOS DE DIRECTORIOS O FICHEROS
*/
#include "bloques.h"
#include "directorios.h"
#include <stdio.h>

int main(int argc, char **argv) {
    //Declaraciones
    struct STAT stat;
    struct tm *ts;
    char buf[80];
    char errbuff[BLOCKSIZE]; //Buffer de errores. 
    int result; 

    //Comprobación de parámetros enviados al programa. 
    if (argc != 3) {
        perror("Error: formato de comando: ./nombre_de_programa "
        "<nombre_de_fichero_disco> </ruta>\n");
        exit(-1);
    }

    //Montaje del disco virtual. 
    if (bmount(argv[1]) == -1) {
        perror("Error: no se ha podido abrir el directorio indicado.\n");
        exit(-1);
    }

    //Inicialización del buffer de errores. 
    memset(errbuff, 0, sizeof(errbuff));

    result = mi_stat(argv[2], &stat) ;

    //Llamada a mi_stat()
    if (result < 0) {
        control_errores_buscar_entrada(result, errbuff);
        fprintf(stderr, "%s", errbuff);
        exit(-1);
    }

    printf("Tipo = %c\n", stat.tipo);
    printf("Permisos = %d\n", stat.permisos);

    ts = localtime(&stat.atime);
    strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", ts);
    printf("atime: %s\n", buf);

    ts = localtime(&stat.ctime);
    strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", ts);
    printf("ctime: %s\n", buf);

    ts = localtime(&stat.mtime);
    strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", ts);
    printf("mtime: %s\n", buf);

    printf("nlinks = %d\n", stat.nlinks);
    printf("TamEnBytesLog = %d\n", stat.tamEnBytesLog);
    printf("NumBloquesOcupados = %d\n", stat.numBloquesOcupados);
    puts("");

    //Desmontaje del dispositivo virtual. 
    if (bumount() == -1) {
        perror("Error: no se ha podido cerrar el fichero.\n");
        exit(-1);
    }
}
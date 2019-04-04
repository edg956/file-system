#include "ficheros.h"
#include "bloques.h"

int main(int argc, char **argv) {

    //Declaraciones. 
    int bliberados; 
    struct STAT STAT;
    struct tm *ts;
    char buf[80];

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

    //Llamada a la función mi_truncar_f().
    bliberados = mi_truncar_f(atoi(argv[2]), atoi(argv[3]));

    if (bliberados==-1) {
        perror("Error: no se ha podido abrir el directorio indicado.\n");
        exit(-1);
    }

    //Mostrar número de bloques liberados.
    printf("Número de bloques liberados: %d\n\n", bliberados);

    //Leer inodo. 
    if (mi_stat_f(atoi(argv[2]), &STAT)==-1) {
        perror("Error: no se ha podido leer el inodo indicado." 
        "Función -> truncar.c - main()\n");
        exit(-1);    
    }

    //Imprimir resultados del inodo
    printf("DATOS INODO %d:\n", atoi(argv[2]));
    printf("Tipo = %c\n", STAT.tipo);
    printf("Permisos = %d\n", STAT.permisos);

    ts = localtime(&STAT.atime);
    strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", ts);
    printf("atime: %s\n", buf);

    ts = localtime(&STAT.ctime);
    strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", ts);
    printf("ctime: %s\n", buf);

    ts = localtime(&STAT.mtime);
    strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", ts);
    printf("mtime: %s\n", buf);

    printf("nlinks = %d\n", STAT.nlinks);
    printf("TamEnBytesLog = %d\n", STAT.tamEnBytesLog);
    printf("NumBloquesOcupados = %d\n", STAT.numBloquesOcupados);

    //Desmontar dispositivo virtual. 
    if (bumount()==-1) {
        perror("Error: no se ha podido desmontar el dispositivo virtual.\n");
        exit(-1);
    }

}

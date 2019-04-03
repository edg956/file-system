#include "ficheros.h"
#include "bloques.h"


int main(int argc, char **argv) {

    bmount(argv[1]);
    struct STAT STAT;
    struct inodo in;
    struct tm *ts;
    char buf[80];

    for (int i = 1; i < 600; i++) {
        mi_stat_f(i, &STAT);
        leer_inodo(i, &in);
        printf("Ninodo: %i STAT.tamEnBytesLog: %u in.tamEnBytesLog: %u\n", i, STAT.tamEnBytesLog, in.tamEnBytesLog);
    
        // //Imprimir resultados
        //     printf("DATOS INODO %d:\n", i);
        //     printf("Tipo = %c\n", STAT.tipo);
        //     printf("Permisos = %d\n", STAT.permisos);

        //     ts = localtime(&STAT.atime);
        //     strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", ts);
        //     printf("atime: %s\n", buf);

        //     ts = localtime(&STAT.ctime);
        //     strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", ts);
        //     printf("ctime: %s\n", buf);

        //     ts = localtime(&STAT.mtime);
        //     strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", ts);
        //     printf("mtime: %s\n", buf);

        //     printf("nlinks = %d\n", STAT.nlinks);
        //     printf("TamEnBytesLog = %d\n", STAT.tamEnBytesLog);
        //     printf("NumBloquesOcupados = %d\n", STAT.numBloquesOcupados);
        
    
    }



    bumount();

    return 0;
}
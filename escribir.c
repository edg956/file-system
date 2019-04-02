/* Escribirá texto en uno o varios inodos haciendo uso de reservar_inodo ('f', 6)
para obtener un nº de inodo, que mostraremos por pantalla y además utilizaremos 
como parámetro para mi_write_f().*/

#include "ficheros.h"
#include "bloques.h"

int main(int argc, char **argv) {

    //Comprobar la sintaxis de la llamada a la función.
    if (argc!=4) {
        perror("Error: Sintaxis de llamada al programa incorrecta.\n"
        "Sintaxis: ./nombre_del_programa <nombre_dispositivo>" 
        "<\"$(cat fichero)\"> <diferentes_inodos>\nOffsets: 0, 5120,"
        "256000, 30720000, 71680000\nSi diferentes inodos = 0 se reserva"
        " un solo inodo para todos los offsets\n");
        exit(-1);
    }

    //Montar dispositivo virtual. 
    if (bmount(argv[1]) == -1) {
        perror("Error: no se ha podido abrir el directorio indicado.\n");
        exit(-1);
    }

    //Declaraciones
    int ninodo; 
    int diferentes_inodos = atoi(argv[3]);
    struct STAT STAT; 
    char buffer[strlen(argv[2])];     
    
    int bytesEscritos = 0;  
    unsigned int arrayOffsets[5] = {0, 5120, 256000, 30720000, 71680000}; 
    struct tm *ts;
    char buf[80];
    
    printf("Longitud texto: %ld\n\n", strlen(argv[2]));

    //Preparar buffer que se utilizará para escribir. 
    //strcpy(&buffer, &argv[2]);

    if(diferentes_inodos==0) {
        //Sólo hay que reservar un inodo en este caso. 

        //Reserva del inodo.
        ninodo = reservar_inodo('f', 6);

        //Comprobar errores de reserva de inodo. 
        if (ninodo == -1) {
            perror("Error al intentar reservar un inodo." 
            "Función -> escribir.c - main()");
            exit(-1);
        }
    
        //Escritura en todos los offsets.
        for(int i = 0; i < sizeof(arrayOffsets)/sizeof(arrayOffsets[0]); i++) {
            strcpy(&buffer[0], argv[2]);                          //Coloco esto aquí y comento la linea antes de este if solo para comprobar la funcionalidad de write_f y read_f
            
            bytesEscritos = mi_write_f(ninodo, &buffer[0], arrayOffsets[i], 
            sizeof(buffer));

            if (bytesEscritos ==-1) {
                perror("Error al intentar reservar un inodo." 
                "Función -> escribir.c - main()");
                exit(-1);
            }

            /*COMPROBACIONES DE CORRECTESA*/
            memset(&buffer[0], 0, sizeof(buffer));
            printf("\nNº inodo reservado: %d\n", ninodo);
            printf("Bytes escritos: %d\n", bytesEscritos);
            bytesEscritos = mi_read_f(ninodo, &buffer[0], arrayOffsets[i], sizeof(buffer));
            //printf("bytes leidos: %i\n", bytesEscritos);
            
            printf("Offset: %u\n", arrayOffsets[i]);
            //printf("Contenido leido: %s\n", &buffer[0]);
            write(1, &buffer[0], sizeof(buffer));  
            puts("");            
            
            //Mostrar datos del inodo escrito. 
            if (mi_stat_f(ninodo, &STAT) < 0) {
                perror("Error: mi_stat_f fallido." 
                "Función -> escribir.c - main()");
                exit(-1);
            }

            //Imprimir resultados
            printf("DATOS INODO %d:\n", ninodo);
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
        }    

    } else {

        //Escribir cada offset en un inodo diferente
        for (int i = 0; i < sizeof(arrayOffsets)/sizeof(arrayOffsets[0]); i++) {
            //Reservar inodo
            ninodo = reservar_inodo('f', 6);

            bytesEscritos = mi_write_f(ninodo, &buffer[0], arrayOffsets[i], 
            sizeof(buffer);
            //Escribir en offset correspondiente a inodo
            if (bytesEscritos<0){
                perror("Error al intentar reservar un inodo." 
                "Función -> escribir.c - main()");
                exit(-1);
            }

            /*COMPROBACIONES DE CORRECTESA*/
            memset(&buffer[0], 0, sizeof(buffer));
            printf("\nNº inodo reservado: %d\n", ninodo);
            printf("Bytes escritos: %d\n", bytesEscritos);
            mi_read_f(ninodo, &buffer[0], arrayOffsets[i], sizeof(buffer));
            //printf("bytes leidos: %i\n", bytesEscritos);
            
            printf("Offset: %u\n", arrayOffsets[i]);
            //printf("Contenido leido: %s\n", &buffer[0]);
            write(1, &buffer[0], sizeof(buffer));  
            puts("");     

            //Mostrar datos del inodo escrito. 
            if (mi_stat_f(ninodo, &STAT) < 0) {
            perror("Error: mi_stat_f fallido." 
                "Función -> escribir.c - main()");
            exit(-1);
            }

            //Imprimir resultados
            printf("DATOS INODO %d:\n", ninodo);
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
        }
    }

    //Desmontar dispositivo virtual. 
    if (bumount()==-1) {
        perror("Error: no se ha podido desmontar el dispositivo virtual.\n");
        exit(-1);
    }

    return 0;
}
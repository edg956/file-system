//FICHERO DE VERIFICACIÓN 
#include "verificacion.h"

int main(int argc, char **argv) {

    //Declaraciones
    struct STAT stat; 
    int nentradas; 
    struct entrada buffentradas[NPROC*sizeof(struct entrada)];
    char dirsimulacion[128]; 
    char *auxBuff;
    struct REGISTRO buffRegistros[OP_ESCR*sizeof(struct REGISTRO)];
    struct INFORMACION info;
    int boolean = 0;
    char *buffEscritura = "";

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
        fprintf(stderr, "Error: El número de entradas es diferente al número de procesos.\n");
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
    
    if (mi_read(argv[2], buffentradas, 0, stat.tamEnBytesLog)==-1) {
        fprintf(stderr, "Error: No se han podido leer las entradas.\n");
        exit(-1);
    }

    for (int i=0; i<nentradas; i++) {
        
        //Leer entrada del directorio. 
        auxBuff = strchr(buffentradas[i].nombre, '_');

        //Extraer el PID a partir del nombre de la entrada y guardarlo en el registro info.        
        info.pid = atoi(&auxBuff[1]);
        
        /*Recorrer secuencialmente el fichero prueba.dat utilizando un buffer 
        de N registros de escrituras*/
        //Copiar el directorio de simulación. 
        strcpy(dirsimulacion, argv[2]);

        if (mi_read(strcat(dirsimulacion, buffentradas[i].nombre), buffRegistros, 0, sizeof(buffRegistros))==-1) {
            fprintf(stderr, "Error: No se ha podido leer el fichero prueba.dat.\n");
            exit(-1);
        }

        for (int j = 0; j < sizeof (buffRegistros)/sizeof(struct REGISTRO); j++) {

            if (info.pid==buffRegistros[i].pid) {

                if (!boolean) {
                    boolean = 1;
                    info.PrimeraEscritura = buffRegistros[i];
                    info.UltimaEscritura = buffRegistros[i];
                    info.MenorPosicion = buffRegistros[i];
                    info.MayorPosicion = buffRegistros[i];
                }else{

                    if (difftime(buffRegistros[i].fecha, info.PrimeraEscritura.fecha) == 0) {

                        if (buffRegistros[i].nEscritura < info.PrimeraEscritura.nEscritura) {
                            info.PrimeraEscritura = buffRegistros[i];
                        }

                        if (buffRegistros[i].nEscritura > info.UltimaEscritura.nEscritura) {
                            info.UltimaEscritura = buffRegistros[i];
                        }

                        if (buffRegistros[i].nRegistro < info.MenorPosicion.nRegistro) {
                            info.MenorPosicion = buffRegistros[i];
                        }

                        if (buffRegistros[i].nRegistro > info.MayorPosicion.nRegistro) {
                            info.MayorPosicion = buffRegistros[i];
                        }

                    }

                }
            }
            info.nEscrituras++;
        }

        boolean = 0;

        //Copiar el directorio de simulación. 
        strcpy(dirsimulacion, argv[2]);
        strcat(dirsimulacion, "informe.txt");

        mi_stat(dirsimulacion, &stat);

        sprintf(buffEscritura, "PID: %d\n Número de escrituras: %d\n"
        "Primera escritura\t%d\t%d\t%s\nÚltima escritura\t%d\t%d\t%s\n"
        "Menor posición\t%d\t%d\t%s\nMayor posición\t%d\t%d\t%s\n\n", 
        info.pid, info.nEscrituras, info.PrimeraEscritura.nEscritura, 
        info.PrimeraEscritura.nRegistro, asctime(localtime(&info.PrimeraEscritura.fecha)), info.UltimaEscritura.nEscritura, 
        info.UltimaEscritura.nRegistro, asctime(localtime(&info.UltimaEscritura.fecha)), info.MenorPosicion.nEscritura, 
        info.MenorPosicion.nRegistro, asctime(localtime(&info.MenorPosicion.fecha)), info.MayorPosicion.nEscritura, 
        info.MayorPosicion.nRegistro, asctime(localtime(&info.MayorPosicion.fecha)));
        
        if (mi_write(dirsimulacion, buffEscritura, stat.tamEnBytesLog, sizeof(buffEscritura))) {
            fprintf(stderr, "Error: No se ha podido escribir el informe.\n");
            exit(-1);
        }

        //Reinciar buffEscritura.
        buffEscritura = "";
    }

    //Desmontar dispositivo virtual
    if (bumount() == -1) {
        fprintf(stderr, "Error: no se ha podido cerrar el fichero(last).\n");
    }

}
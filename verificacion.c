//FICHERO DE VERIFICACIÓN 
#include "verificacion.h"

int main(int argc, char **argv) {

    //Declaraciones
    struct STAT stat;
    int nentradas;
    int offset = 0;
    int cant_registros_buffer_escrituras = 256;
    struct entrada buffentradas[NPROC*sizeof(struct entrada)];
    char dirsimulacion[128]; 
    char *auxBuff;
    struct REGISTRO buffRegistros[cant_registros_buffer_escrituras];
    struct INFORMACION info;
    //int boolean = 0;
    char buffEscritura[1024], str1[26], str2[26], str3[26], str4[26]; //Buffers
    int j;

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
        fprintf(stderr, "Error: No se ha podido obtener el stat.\n");
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

    //Limpiar buffer
    memset(buffEscritura, 0, sizeof(buffEscritura));

    //MEJORA
    //Lectura de todas las entradas entes de entrar al bucle para almacenarlas
    //en un buffer. 
    
    if (mi_read(argv[2], buffentradas, 0, stat.tamEnBytesLog)==-1) {
        fprintf(stderr, "Error: No se han podido leer las entradas.\n");
        exit(-1);
    }

    for (int i=0; i<nentradas; i++) {
        
        info.nEscrituras = 0;
        //Leer entrada del directorio. 
        auxBuff = strchr(buffentradas[i].nombre, '_');

        //Extraer el PID a partir del nombre de la entrada y guardarlo en el registro info.        
        info.pid = atoi(&auxBuff[1]);

        //Resetear directorio de simulación.
        memset(dirsimulacion, 0, sizeof(dirsimulacion));

        //Cargar nuevo directorio.
        sprintf(dirsimulacion, "%sproceso_%d/prueba.dat", argv[2], info.pid);

        /*Recorrer secuencialmente el fichero prueba.dat utilizando un buffer 
        de N registros de escrituras*/

        while (mi_read(dirsimulacion, buffRegistros, offset, sizeof(buffRegistros)) > 0) { 

            for (j = 0; j < sizeof(buffRegistros)/sizeof(struct REGISTRO) && info.nEscrituras < OP_ESCR; j++) {
                
                //Comprobar que la escritura sea válida. 
                if (info.pid==buffRegistros[j].pid) {

                    //Detección de la primera escritura validada. 
                    if (info.nEscrituras==0) {
                        info.PrimeraEscritura = buffRegistros[j];
                        info.UltimaEscritura = buffRegistros[j];
                        info.MenorPosicion = buffRegistros[j];
                        info.MayorPosicion = buffRegistros[j];

                    } else {
                        
                        //Comparación de la fecha de los registros. 
                        if (difftime(buffRegistros[j].fecha, info.PrimeraEscritura.fecha) <= 0) {
                            //Comparación del número de escritura del registro (en caso de fecha igual). 
                            if (buffRegistros[j].nEscritura < info.PrimeraEscritura.nEscritura) {
                                info.PrimeraEscritura = buffRegistros[j];
                            }
                       }

                        if (difftime(buffRegistros[j].fecha, info.UltimaEscritura.fecha) >= 0) {
                            //Comparación del número de escritura del registro (en caso de fecha igual). 
                            if (buffRegistros[j].nEscritura > info.UltimaEscritura.nEscritura) {
                                info.UltimaEscritura = buffRegistros[j];
                            }
                        }
                    }
                    info.nEscrituras++;

                    //Obtener la escritura de la última posición.
                    info.MayorPosicion = buffRegistros[j];
                }
            }

            //Limpiar buffer
            memset(buffRegistros, 0, sizeof(buffRegistros));
            offset+=sizeof(buffRegistros);
        }

        printf("%d) %d escrituras validadas en %s\n", i+1, info.nEscrituras, dirsimulacion);

        //Copiar el directorio de simulación. 
        strcpy(dirsimulacion, argv[2]);
        strcat(dirsimulacion, "informe.txt");

        mi_stat(dirsimulacion, &stat);

        //Preparar strings de fecas para buffer de escritura
        asctime_r(localtime(&info.PrimeraEscritura.fecha), str1);
        asctime_r(localtime(&info.UltimaEscritura.fecha), str2);
        asctime_r(localtime(&info.MenorPosicion.fecha), str3);
        asctime_r(localtime(&info.MayorPosicion.fecha), str4);

        //Añadir información del struct info para imprimirla por pantalla. 
        sprintf(buffEscritura, "PID: %d\nNúmero de escrituras:\t%d\n"
        "Primera escritura:\t%d\t%d\t%sÚltima escritura:\t%d\t%d\t%s"
        "Menor posición:\t\t%d\t%d\t%sMayor posición:\t\t%d\t%d\t%s\n", 
        info.pid, info.nEscrituras, info.PrimeraEscritura.nEscritura, 
        info.PrimeraEscritura.nRegistro, str1, info.UltimaEscritura.nEscritura, 
        info.UltimaEscritura.nRegistro, str2, info.MenorPosicion.nEscritura, 
        info.MenorPosicion.nRegistro, str3, info.MayorPosicion.nEscritura, 
        info.MayorPosicion.nRegistro, str4);

        //Escribir el informe.txt
        if (mi_write(dirsimulacion, buffEscritura, stat.tamEnBytesLog, sizeof(buffEscritura))==-1) {
            fprintf(stderr, "Error: No se ha podido escribir el informe.\n");
            exit(-1);
        }

        //Reinciar buffEscritura.
        memset(buffEscritura, 0, sizeof(buffEscritura));
        memset(buffRegistros, 0, sizeof(buffRegistros));
        offset = 0;
    }

    //Desmontar dispositivo virtual
    if (bumount() == -1) {
        fprintf(stderr, "Error: no se ha podido cerrar el fichero(last).\n");
    }
return 0;
}
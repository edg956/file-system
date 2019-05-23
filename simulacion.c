//PROGRAMA DE SIMULACIÓN. 

#include "simulacion.h"

int acabados = 0;

int main(int argc, char ** argv) {

    //Declaraciones. 
    int pid;
    struct REGISTRO registro;

    time_t tiempo = time(0);
    struct tm * tlocal = localtime( & tiempo);
    char ruta_principal[128];
    char ruta_aux[128];

    strftime(ruta_principal, 128, "/simul_%Y%m%d%H%M%S/", tlocal);

    //Comprobar el número de argumentos. 
    if (argc != 2) {
        fprintf(stderr, "Error: Sintaxis: ./nombre_de_programa <disco>\n");
        exit(-1);
    }

    //Montar disco. 
    if (bmount(argv[1]) == -1) {
        fprintf(stderr, "Error: El disco no se ha podido montar. \n");
        exit(-1);
    }

    //Crear directorio de simulación en la raíz /simul_aaaammddhhmmss/
    if (mi_creat(ruta_principal, 6) == -1) {
        fprintf(stderr, "Error: No se ha podido crear el directorio de simulación\n");
        if (bumount() == -1) {
            fprintf(stderr, "Error: no se ha podido cerrar el fichero.\n");
        }
        exit(-1);
    }

    //MENSAJES DE INFORMACIÓN DE EJECUCIÓN NIVEL 13------------------------------------------------------------>
    printf("Directorio simulación: %s\n", ruta_principal);

    //Asignar la función de enterrador (reaper) a la señal de finalización de 
    //un hijo (SIGCHLD). 
    signal(SIGCHLD, reaper);

    for (int proceso = 1; proceso <= NPROC; proceso++) {

        //Se crea un nuevo proceso. 
        pid = fork();

        if (pid == -1) {
           fprintf(stderr, "Error: no se ha podido crear el proceso.\n");
           exit(-1);
        }

        if (!pid) {

            //Montar disco nuevamente. 
            if (bmount(argv[1]) == -1) {
                fprintf(stderr, "Error: El disco no se ha podido montar. \n");
                exit(-1);
            }

            //Construir directorio del proceso. 
            sprintf(ruta_aux, "proceso_%d/", getpid());

            //Concatenar la ruta principal con la del directorio. 
            strcat(ruta_principal, ruta_aux);

            //Crear el directorio del proceso.
            if (mi_creat(ruta_principal, 6) == -1) {
                fprintf(stderr, "Error: No se ha podido crear el directorio del proceso.\n");
                if (bumount() == -1) {
                    fprintf(stderr, "Error: no se ha podido cerrar el fichero.\n");
                }
                exit(-1);
            }
                
            //Añadir el fichero prueba.dat a la ruta principal. 
            strcat(ruta_principal, "prueba.dat");

            //Crear fichero prueba dentro del directorio del proceso. 
            if (mi_touch(ruta_principal, 6) == -1) {
                fprintf(stderr, "Error: No se ha podido crear el directorio de simulación\n");
                if (bumount() == -1) {
                    fprintf(stderr, "Error: no se ha podido cerrar el fichero.\n");
                }
                exit(-1);
            }

            //Inicializar la semilla para los números aleatorios. 
            srand(time(NULL) + getpid());

            for (int i = 0; i < OP_ESCR; i++) {

                //Inicializar el registro. 
                registro.fecha = time(NULL);
                registro.pid = getpid();
                registro.nEscritura = i + 1;
                registro.nRegistro = rand() % REGMAX;
                    
                //MENSAJES DE INFORMACIÓN DE EJECUCIÓN NIVEL 13------------------------------------------------------------>
                //printf("[simulación.c -> Escritura %d en %s]\n", i+1, ruta_principal);
                
                strcpy(ruta_aux, ruta_principal);

                if (mi_write(ruta_principal, &registro, registro.nRegistro * sizeof(struct REGISTRO), sizeof(struct REGISTRO)) == -1) {
                    fprintf(stderr, "Error: No se ha podido escribir el registro. \n");
                    exit(-1);
                }

                //Esperar 0,05 segundos. (50ms = 50000us)
                usleep(50000);
            }

            //MENSAJES DE INFORMACIÓN DE EJECUCIÓN NIVEL 13------------------------------------------------------------>
            printf("[Proceso %d: Completadas %d escrituras en %s]\n", proceso, OP_ESCR, ruta_principal);

            //Desmontar dispositivo virtual (proceso hijo).
            if (bumount() == -1) {
                fprintf(stderr, "Error: no se ha podido cerrar el fichero.\n");
            }

            //Reiniciar strings. 
            ruta_aux[0] = 0;
            ruta_principal[0] = 0;

            exit(0);

        }

        //Esperar 0,2 seg
        usleep(200000);
    }

    //Permitir que el padre espere por todos los hijos: 
    while (acabados < NPROC){
        pause();
    }

    //Desmontar dispositivo virtual
    if (bumount() == -1) {
        fprintf(stderr, "Error: no se ha podido cerrar el fichero(last).\n");
    }
    printf("Total de procesos terminados: %d.\n",acabados);
    exit(0);
}

void reaper() {

    pid_t ended;
    signal(SIGCHLD, reaper);
    while ((ended = waitpid(-1, NULL, WNOHANG)) > 0) {
        acabados++;
        //Podemos testear qué procesos van acabando:
        //fprintf(stderr, "[simulación.c -> Acabado proceso con PID %d, total acabados: %d\n", ended, acabados);
    }
}
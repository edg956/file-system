//PROGRAMA DE SIMULACIÓN. 

#include "simulacion.h"

int acabados = 0;

int main(int argc, char ** argv) {

    //Declaraciones. 
    int
    const NPROC = 3;
    int
    const OP_ESCR = 10;
    int pid;
    struct REGISTRO registro;

    time_t tiempo = time(0);
    struct tm * tlocal = localtime( & tiempo);
    char formato_directorio_simulacion[128];
    char formato_directorio_proceso[128];

    strftime(formato_directorio_simulacion, 128, "/simul_%Y%m%d%H%M%S/", tlocal);

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
    if (mi_creat(formato_directorio_simulacion, 6) == -1) {
        fprintf(stderr, "Error: No se ha podido crear el directorio de simulación\n");
        if (bumount() == -1) {
            fprintf(stderr, "Error: no se ha podido cerrar el fichero.\n");
        }
        exit(-1);
    }

    //MENSAJES DE INFORMACIÓN DE EJECUCIÓN NIVEL 13------------------------------------------------------------>
    printf("Directorio simulación: %s\n", formato_directorio_simulacion);

    //Asignar la función de enterrador (reaper) a la señal de finalización de 
    //un hijo (SIGCHLD). 
    signal(SIGCHLD, reaper);
    printf("PID padre: %d\n", getpid());
    for (int proceso = 1; proceso <= NPROC; proceso++) {

        //Se crea un nuevo proceso. 
        pid = fork();
        printf("PID: %d\n", getpid());

        if (pid == -1) {
           fprintf(stderr, "Error: no se ha podido crear el proceso.\n");
           exit(-1);
        }
        
        if (getpid()) {

            //Montar disco nuevamente. 
            if (bmount(argv[1]) == -1) {
                fprintf(stderr, "Error: El disco no se ha podido montar. \n");
                exit(-1);
            }

            sprintf(formato_directorio_proceso, "proceso_%d/", pid);

            // printf("FDP: %s\n", formato_directorio_proceso);
            // printf("FDS: %s\n", formato_directorio_simulacion);

            strcat(formato_directorio_simulacion, formato_directorio_proceso);

            // printf("FDP: %s\n", formato_directorio_proceso);
            // printf("FDS: %s\n", formato_directorio_simulacion);       

            //Crear el directorio del proceso.
            if (mi_creat(formato_directorio_simulacion, 6) == -1) {
                fprintf(stderr, "Error: No se ha podido crear el directorio del proceso.\n");
                if (bumount() == -1) {
                    fprintf(stderr, "Error: no se ha podido cerrar el fichero.\n");
                }
                exit(-1);
            }

            //printf("FDS: %s\n", formato_directorio_simulacion);
                
                strcat(formato_directorio_simulacion, "prueba.dat");
                //Crear fichero prueba dentro del directorio del proceso. 
                if (mi_touch(formato_directorio_simulacion, 6) == -1) {
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

                    //Escribir registro. 
                    strcat(formato_directorio_proceso, "prueba.dat");
                    
                    //MENSAJES DE INFORMACIÓN DE EJECUCIÓN NIVEL 13------------------------------------------------------------>
                    printf("[simulación.c -> Escritura %d en %s]\n", i, formato_directorio_proceso);
                   
                    if (mi_write(formato_directorio_proceso, & registro, registro.nRegistro * sizeof(struct REGISTRO), sizeof(struct REGISTRO) == -1)) {
                        fprintf(stderr, "Error: No se ha podido escribir el registro. \n");
                        exit(-1);
                    }

                    //Esperar 0,05 segundos. (50ms = 50000us)
                    usleep(50000);
                }

                //MENSAJES DE INFORMACIÓN DE EJECUCIÓN NIVEL 13------------------------------------------------------------>
                printf("[Proceso %d: Completadas %d escrituras en %s]", proceso, OP_ESCR, strcat("prueba.dat", formato_directorio_proceso));

                //Desmontar dispositivo virtual (proceso hijo).
                if (bumount() == -1) {
                    fprintf(stderr, "Error: no se ha podido cerrar el fichero.\n");
                }
                exit(0);
            }
            puts("endif");
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
        exit(0);
}

void reaper() {

    pid_t ended;
    signal(SIGCHLD, reaper);
    while ((ended = waitpid(-1, NULL, WNOHANG)) > 0) {
        acabados++;
        //Podemos testear qué procesos van acabando:
        fprintf(stderr, "acabado: %d total acabados: %d\n", ended, acabados);
    }

}
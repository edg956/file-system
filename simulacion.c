
//PROGRAMA DE SIMULACIÓN. 

#include "simulacion.h"

int main(int argc, char **argv) {

    //Declaraciones. 
    int const NPROC = 100; 
    int const OP_ESCR = 50;
    int pid; 
    struct REGISTRO registro; 

    time_t tiempo = time(0);
    struct tm *tlocal = localtime(&tiempo);
    char formato_directorio_simulacion[128];
    char formato_directorio_proceso[128]; 
    char formato_directorio_proceso_n[128];

    strftime(formato_directorio_simulacion,128,"/simul_%Y%m%d%H%M%S/",tlocal);

    //Comprobar el número de argumentos. 
    if (argc!=2) {
        fprintf(stderr, "Error: Sintaxis: ./nombre_de_programa <disco>\n");
        exit(-1);
    }

    //Montar disco. 
    if (bmount(argv[1])==-1) {
        fprintf(stderr, "Error: El disco no se ha podido montar. \n");
        exit(-1);
    }

    //Crear directorio de simulación en la raíz /simul_aaaammddhhmmss/
    if (mi_creat(formato_directorio_simulacion, 6)==-1) {
        fprintf(stderr, "Error: No se ha podido crear el directorio de simulación\n");
        if (bumount() == -1) {
            fprintf(stderr, "Error: no se ha podido cerrar el fichero.\n");
        }
        exit(-1);
    }

    //Asignar la función de enterrador (reaper) a la señal de finalización de 
    //un hijo (SIGCHLD). 
    signal(SIGCHLD, reaper);

    for (int proceso = 1; proceso <= NPROC; proceso++) {
        
        //Se crea un nuevo proceso. 
        if (pid=fork()==-1) {
            fprintf(stderr, "Error: no se ha podido crear el proceso.\n");
            exit(-1);
        }

        if (pid==0) {

            //Montar disco nuevamente. 
            if (bmount(argv[1])==-1) {
                fprintf(stderr, "Error: El disco no se ha podido montar. \n");
                exit(-1);
            }

            strcpy(formato_directorio_proceso, "proceso_"+pid);
            strcat("/", formato_directorio_proceso);

            //Crear el directorio del proceso.
            if (mi_creat(strcat(formato_directorio_proceso, formato_directorio_simulacion), 6) ==-1) {
                fprintf(stderr, "Error: No se ha podido crear el directorio del proceso.\n");
                    if (bumount() == -1) {
                        fprintf(stderr, "Error: no se ha podido cerrar el fichero.\n");
                    }
                exit(-1);
                        

            //Crear fichero prueba dentro del directorio del proceso. 
            if (mi_touch(strcat("prueba.dat", formato_directorio_proceso), 6)==-1) {
                fprintf(stderr, "Error: No se ha podido crear el directorio de simulación\n");
                    if (bumount() == -1) {
                        fprintf(stderr, "Error: no se ha podido cerrar el fichero.\n");
                    }
                exit(-1);
            }

            //Inicializar la semilla para los números aleatorios. 

            srand(time(NULL)+getpid());

            for (int i = 0; i < OP_ESCR; i++) {

                //Inicializar el registro. 
                registro.fecha=time(NULL);
                registro.pid = getpid();
                registro.nEscritura = i+1; 
                registro.nRegistro = rand() % REGMAX; 

                //Escribir registro. 
                if (mi_write(strcat("prueba.dat", formato_directorio_proceso), &registro, registro.nRegistro*sizeof(struct REGISTRO), sizeof(struct REGISTRO)==-1) {
                    fprintf(stderr, "Error: No se ha podido escribir el registro. \n");
                    exit(-1);
                }

                //Esperar 0,05 segundos.
                usleep();


            }
        }
    }
    
    //Desmontar dispositivo virtual
    if (bumount() == -1) {
        fprintf(stderr, "Error: no se ha podido cerrar el fichero.\n");
    }
}

void reaper(int signum) {

    signal(SIGCHLD, reaper);

    pid_t pid;
    int wstatus;

    pid = wait(NULL);
    printf("Pid %d terminado.\n", pid);

}

#include <stdio.h>
#include "directorios.h"
#include "bloques.h"
#include <time.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

#define REGMAX 500000

int const NPROC = 100;
int const OP_ESCR = 50;

struct REGISTRO {
    time_t fecha; //fecha de la escritura en formato epoch 
    pid_t pid; //PID del proceso que lo ha creado
    int nEscritura; //Entero con el número de escritura (de 1 a 50)
    int nRegistro; //Entero con el número del registro dentro del fichero (de 0 a REGMAX-1) 
};

void reaper();
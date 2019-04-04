/*Le pasaremos por línea de comandos un nº de inodo obtenido con el programa 
anterior (además del nombre del dispositivo). Su funcionamiento tiene que ser 
similar a la función cat de linux, explorando TODO el fichero*/

#include "ficheros.h"
#include "bloques.h"
#include <stdio.h>

#define BUF_SIZE BLOCKSIZE

int main(int argc, char **argv) {

    //Comprobación de los argumentos pasados al programa. 
    if (argc!=3) {
        fprintf(stderr,"Error: Sintaxis de llamada al programa incorrecta.\n"
        "Sintaxis: ./nombre_del_programa <nombre_dispositivo>" 
        "<numero_inodo>");
        exit(-1);       
    }

    //Montar dispositivo virtual. 
    if (bmount(argv[1]) == -1) {
        perror("Error: no se ha podido abrir el directorio indicado.\n");
        exit(-1);
    }
    
    int ninodo = atoi(argv[2]);
    int bytes_leidos = 0;
    int aux, offset = 0;
    unsigned char buffer[BUF_SIZE];
    struct STAT STAT;
    //Variables para imprimir mensajes sobre bytes leidos a través de stdout
    char str[256];

    //Inicializar buffer con memset
    memset(buffer, 0, BUF_SIZE);

    sprintf(&str[0], "Lectura de inodo %i:\n",ninodo);
    write(2, &str, strlen(str));

    //Primera lectura de fichero
    aux = mi_read_f(ninodo, buffer, offset, BUF_SIZE);

    //Lectura del inodo
    if (mi_stat_f(ninodo, &STAT) < 0) {
        perror("Error: no se ha podido obtener información del inodo."
        "Función -> leer.c - main()");
        exit(-1);
    }

    //Recorrido al contenido del inodo
    while (aux > 0) {
        //Escribir por consola
        write(1, buffer, aux);
        //Reset buffer para próxima lectura
        memset(buffer, 0, BUF_SIZE);
        //Aumentar Nº de bytes leidos
        bytes_leidos += aux;
        //Siguiente lectura de fichero
        offset += BUF_SIZE;
        aux = mi_read_f(ninodo, buffer, offset, BUF_SIZE);
    }
    
    if (aux==-1) {
        perror("Error: no se ha podido leer del inodo."
        "Función -> leer.c - main()");
        exit(-1);           
    }

    sprintf(&str[0], "\ntotal_leidos %i\nTamEnBytesLog "
    "%i\n",bytes_leidos, STAT.tamEnBytesLog);
    write(2, &str, strlen(str));

    //Desmontar dispositivo virtual. 
    if (bumount()==-1) {
        perror("Error: no se ha podido desmontar el dispositivo virtual.\n");
        exit(-1);
    }
}
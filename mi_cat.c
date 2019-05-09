#include "directorios.h"
#include <string.h>

/*
Programa (comando) que muestra TODO el contenido de un fichero.
    - Comprovar que la ruta se corresponde con la de un fichero y no a la de un directorio
    - Han de coincidir los bytes leidos con el tamaño en bytes lógicos del fichero y con el tamaño físico
    del fichero externo al que redireccionemos la lectura, y se ha de filtrar la basura
    - Utilizar una variable tambuffer para poder cambiar el tamaño del buffer de lectura sin tener que modificar 
    todas las sentencias involucradas.
*/
int main (int argc, char **argv){

int bytesLeidos;
int totalBytesLeidos = 0;
unsigned int offset = 0;
unsigned char tamBuffer[BLOCKSIZE];
struct STAT stat;
char *camino = argv[2];

if (argc != 3){
    perror("Error: número de argumentos incorrecto.\n");
    exit(-1);
}
//Comprobamos que la ruta sea la de un fichero
if (camino [strlen(camino) - 1] == '/'){    
    perror("Error: La ruta no es un fichero.\n");
    exit(-1);
}    

//Montaje del disco virtual. 
    if (bmount(argv[1]) == -1) {
        fprintf(stderr, "Error: no se ha podido abrir el directorio indicado.\n");
        exit(-1);
    }

memset(tamBuffer, 0, BLOCKSIZE);

//Pasamos por todos los bloques para leer los bytes
while ((bytesLeidos = mi_read_f(*camino, tamBuffer, offset, BLOCKSIZE)) > 0){
    write(1, tamBuffer, bytesLeidos);
    memset(tamBuffer, 0, BLOCKSIZE);
    offset += BLOCKSIZE;
    totalBytesLeidos += bytesLeidos;
}
printf("Bytes Leidos: %i\n", totalBytesLeidos);
mi_stat_f(*camino, &stat);
printf ("tamEnBytesLog: %i\n", stat.tamEnBytesLog);
    
//Desmontaje del dispositivo virtual. 
    if (bumount() == -1) {
        fprintf(stderr, "Error: no se ha podido cerrar el fichero.\n");
        exit(-1);
    }

return 0;    
}
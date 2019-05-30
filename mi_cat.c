#include "directorios.h"
#include <string.h>

#define BUF_SIZE BLOCKSIZE*4

/*
Programa (comando) que muestra TODO el contenido de un fichero.
    - Comprobar que la ruta se corresponde con la de un fichero y no a la de
    un directorio.

    - Han de coincidir los bytes leidos con el tamaño en bytes lógicos del 
    fichero y con el tamaño físico del fichero externo al que redireccionemos 
    la lectura, y se ha de filtrar la basura.

    - Utilizar una variable tambuffer para poder cambiar el tamaño del buffer 
    de lectura sin tener que modificar todas las sentencias involucradas.
*/
int main (int argc, char **argv) {

    //Declaraciones
    int bytesLeidos;
    int totalBytesLeidos = 0;
    unsigned int offset = 0;
    unsigned char tamBuffer[BUF_SIZE];
    char *camino = argv[2];
    char errbuff[BLOCKSIZE]; 
    
    //Inicializar buffers. 
    memset(errbuff, 0, BLOCKSIZE);
    memset(tamBuffer, 0, BUF_SIZE);

    //Verificación de los parámetros pasados en la llamada del programa. 
    if (argc != 3){
        fprintf(stderr, "Error: número de argumentos incorrecto.\n"
        "Sintaxis: ./mi_cat <disco> </ruta_fichero>");
        exit(-1);
    }

    //Comprobamos que la ruta sea la de un fichero.
    if (camino [strlen(camino) - 1] == '/'){    
        fprintf(stderr, "Error: La ruta no es un fichero.\n");
        exit(-1);
    }    

    //Montaje del disco virtual. 
    if (bmount(argv[1]) == -1) {
        fprintf(stderr, "Error: no se ha podido abrir el directorio indicado.\n");
        exit(-1);
    }

    //Pasamos por todos los bloques para leer los bytes
    while ((bytesLeidos = mi_read(camino, tamBuffer, offset, BUF_SIZE)) > 0){
        write(1, tamBuffer, bytesLeidos);
        memset(tamBuffer, 0, BUF_SIZE);
        offset += BUF_SIZE;
        totalBytesLeidos += bytesLeidos;
    }

    //Verificar algún error en buscar_entrada. 
    if (bytesLeidos < 0) {
        control_errores_buscar_entrada(bytesLeidos, errbuff);
        exit(-1);
    }

    //Mostrar por pantalla cantidad de bytes leídos. 
    fprintf(stderr, "\nBytes Leidos: %i\n", totalBytesLeidos);
    
    //Desmontaje del dispositivo virtual. 
    if (bumount() == -1) {
        fprintf(stderr, "Error: no se ha podido cerrar el fichero.\n");
        exit(-1);
    }
    return 0;    
}
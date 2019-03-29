/*Le pasaremos por línea de comandos un nº de inodo obtenido con el programa 
anterior (además del nombre del dispositivo). Su funcionamiento tiene que ser 
similar a la función cat de linux, explorando TODO el fichero*/


#include "ficheros_basico.h"
#include "ficheros.h"
#include "bloques.h"

int main(int argc, char **argv) {

    //Comprobación de los argumentos pasados al programa. 
    if (argc!=2) {
        perror("Error: Sintaxis de llamada al programa incorrecta.\n"
        "Sintaxis: ./nombre_del_programa <nombre_dispositivo>" 
        "<numero_inodo>");
        exit(-1);       
    }

    //Montar dispositivo virtual. 
    if (bmount(argv[1]) == -1) {
        perror("Error: no se ha podido abrir el directorio indicado.\n");
        exit(-1);
    }
    
    //Desmontar dispositivo virtual. 
    if (bumount()==-1) {
        perror("Error: no se ha podido desmontar el dispositivo virtual.\n");
        exit(-1);
    }
}
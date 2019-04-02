#include "ficheros.h"
#include "bloques.h"

int main(int argc, char **argv) {

    //Comprobar la sintaxis de la llamada a la función.
    if (argc!=4) {
        perror("Error: Sintaxis de llamada al programa incorrecta.\n"
        "Sintaxis: ./nombre_del_programa <nombre_dispositivo>" 
        "<ninodo> <permisos\n>");
        exit(-1);
    }

    //Montar dispositivo virtual. 
    if (bmount(argv[1]) == -1) {
        perror("Error: no se ha podido abrir el directorio indicado.\n");
        exit(-1);
    }

    unsigned char permisos = *argv[3];

    //Llamada a mi_chmod_f().
    if (mi_chmod_f(atoi(argv[2]), permisos) ==-1) {
        perror("Error: No se han podido asignar los permisos correspondientes.\n");
        exit(-1); 
    }

    //Desmontar dispositivo virtual. 
    if (bumount()==-1) {
        perror("Error: no se ha podido desmontar el dispositivo virtual.\n");
        exit(-1);
    }

}
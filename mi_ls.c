/*
    PROGRAMA PARA LISTAR LOS DIRECTORIOS O FICHEROS DE LA RUTA INDICADA
*/
#include "bloques.h"
#include "directorios.h"

<<<<<<< HEAD
#define BUF_SIZE    4096
#define AUX_BUF_SIZE  128

int main(int argc, char **argv) {

    char buffer[BLOCKSIZE];
    char buffer_aux[AUX_BUF_SIZE];
    int cont = 0;
    int cont2 = 0;

=======
int main(int argc, char **argv) {

>>>>>>> Implementadas funciones casi totalmente. Creados ficheros de programas de nivel 9 y agregados al makefile.
    //Comprobación de parámetros enviados al programa. 
    if (argc != 3) {
        perror("Error: formato de comando: ./nombre_de_programa <nombre_de_fichero_disco> </ruta_directorio>\n");
        exit(-1);
    }

<<<<<<< HEAD
    //Montaje del disco virtual.
=======
    //Montaje del disco virtual. 
>>>>>>> Implementadas funciones casi totalmente. Creados ficheros de programas de nivel 9 y agregados al makefile.
    if (bmount(argv[1]) == -1) {
        perror("Error: no se ha podido abrir el directorio indicado.\n");
        exit(-1);
    }

<<<<<<< HEAD
    //Leer el directorio y llenar buffer
    cont = mi_dir(argv[1], buffer);
    if (cont == -1) {
        perror("Error: no se ha podido leer el directorio.");
        exit(-1);
    }

    //Impresion por pantalla de cabecera
    printf("Total entradas: %i\n", cont);
    puts("Tipo  Permisos    mTime           Tamaño  Nombre");
    puts("---------------------------------------------------");

    cont = 0; //Índice
    //Tratamiento del buffer
    while (buffer[cont] != '\0' && cont < BUF_SIZE) {
        //Si es caracter de separación, imprimir buffer auxiliar
        if (buffer[cont] == '|') {
            printf("%s\n", buffer_aux);
            cont2 = 0;
        } else {
            //Seguir copiando del buffer al buffer auxiliar
            if (cont2 < AUX_BUF_SIZE) {
                buffer_aux[cont2] = buffer[cont];
            }
        }
        cont++; //Aumentar contadores
        cont2++;
    }

    //Desmontaje del dispositivo virtual.
=======

    //Desmontaje del dispositivo virtual. 
>>>>>>> Implementadas funciones casi totalmente. Creados ficheros de programas de nivel 9 y agregados al makefile.
    if (bumount() == -1) {
        perror("Error: no se ha podido cerrar el fichero.\n");
        exit(-1);
    }
}
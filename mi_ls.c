/*
    PROGRAMA PARA LISTAR LOS DIRECTORIOS O FICHEROS DE LA RUTA INDICADA
*/
#include "bloques.h"
#include "directorios.h"

#define BUF_SIZE    4096
#define AUX_BUF_SIZE  128

int main(int argc, char **argv) {

    char buffer[BLOCKSIZE];
    char buffer_aux[AUX_BUF_SIZE];
    int cont = 0;
    int cont2 = 0;
    char errbuff[BLOCKSIZE]; //Buffer de errores. 

    //Comprobación de parámetros enviados al programa. 
    if (argc != 3) {
        fprintf(stderr, "Error: formato de comando: ./nombre_de_programa <nombre_de_fichero_disco> </ruta_directorio>\n");
        exit(-1);
    }

    //Montaje del disco virtual.
    if (bmount(argv[1]) == -1) {
        fprintf(stderr, "Error: no se ha podido abrir el directorio indicado.\n");
        exit(-1);
    }

    //Limpiar buffers
    memset(buffer, 0, BLOCKSIZE);
    memset(buffer_aux, 0, BLOCKSIZE);

    //Inicialización del buffer de errores.
    memset(errbuff, 0, sizeof(errbuff));

    //Leer el directorio y llenar buffer
    cont = mi_dir(argv[2], buffer);
    if (cont < 0) {
        control_errores_buscar_entrada(cont, errbuff);
        fprintf(stderr, "%s", errbuff);    
        exit(-1);
    }

    //Impresion por pantalla de cabecera
    printf("Total entradas: %i\n", cont);
    puts("Tipo  Permisos    mTime                  Tamaño  Nombre");
    puts("------------------------------------------------------------");

    cont = 0; //Índice
    //Tratamiento del buffer
    while (buffer[cont] != '\0' && cont < BUF_SIZE) {
        //Si es caracter de separación, imprimir buffer auxiliar
        if (buffer[cont] == '|') {
            printf("%s\n", buffer_aux);
            cont2 = -1;
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
    if (bumount() == -1) {
        fprintf(stderr, "Error: no se ha podido cerrar el fichero.\n");
        exit(-1);
    }
}
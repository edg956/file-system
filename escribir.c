/* Escribirá texto en uno o varios inodos haciendo uso de reservar_inodo ('f', 6)
para obtener un nº de inodo, que mostraremos por pantalla y además utilizaremos 
como parámetro para mi_write_f().*/

#include "ficheros_basico.h"
#include "ficheros.h"

int main(int argc, char **argv) {

    //Comprobar la sintaxis de la llamada a la función.
    if (argc!=3) {
        perror("Error: Sintaxis de llamada al programa incorrecta.\n"
        "Sintaxis: ./nombre_del_programa <nombre_dispositivo>" 
        "<""$(cat fichero)""> <diferentes_inodos\n>");
        exit(-1);
    }

    //Montar dispositivo virtual. 
    if (bmount(argv[1]) == -1) {
        perror("Error: no se ha podido abrir el directorio indicado.\n");
        exit(-1);
    }

    //Declaraciones
    int ninodo; 
    int diferentes_inodos = argv[3];
    struct STAT STAT; 
    unsigned char buffer[strlen(argv[2])];     
    unsigned int arrayOffsets[5] = {0, 5120, 256000, 30720000, 71680000};   

    //Preparar buffer que se utilizará para escribir. 
    strcpy(&buffer, &argv[2]);

    if(diferentes_inodos==0) {
    //Sólo hay que reservar un inodo en este caso. 

    //Reserva del inodo.
    ninodo = reservar_inodo('f', 6);

    //Comprobar errores de reserva de inodo. 
    if (ninodo == -1) {
        perror("Error al intentar reservar un inodo." 
        "Función -> escribir.c - main()");
        exit(-1);
    }

    //Mostrar el número de inodo por pantalla. 
    printf("El número de inodo reservado es: %d\n", ninodo);
    
    //Escritura en todos los offsets. 

    for(int i = 0; i < sizeof(arrayOffsets); i++) {

        if (mi_write_f(ninodo, &buffer, arrayOffsets[i], sizeof(buffer))==-1) {
            perror("Error al intentar reservar un inodo." 
            "Función -> escribir.c - main()");
            exit(-1);
        }
    }
    




    //Mostrar datos del inodo escrito. 
    mi_stat_f(ninodo, &STAT);

    printf("Tamaño en bytes lógico del inodo: %d\n", STAT.tamEnBytesLog);
    printf("Número de bloques ocupados: %d\n", STAT.numBloquesOcupados);


    }else{



        
    }

    //Desmontar dispositivo virtual. 
    if (bumount()==-1) {
        perror("Error: no se ha podido desmontar el dispositivo virtual.\n");
        exit(-1);
    }
}
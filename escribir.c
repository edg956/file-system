/* Escribirá texto en uno o varios inodos haciendo uso de reservar_inodo ('f', 6)
para obtener un nº de inodo, que mostraremos por pantalla y además utilizaremos 
como parámetro para mi_write_f().*/

<<<<<<< HEAD
#include "ficheros_basico.h"
#include "ficheros.h"
#include "bloques.h"


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
    int diferentes_inodos = atoi(&argv[3]);
    struct STAT STAT; 
    unsigned char buffer[strlen(argv[2])];     
    unsigned int arrayOffsets[5] = {0, 5120, 256000, 30720000, 71680000};   

    //Preparar buffer que se utilizará para escribir. 
    //strcpy(&buffer, &argv[2]);

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
        for(int i = 0; i < sizeof(arrayOffsets)/sizeof(arrayOffsets[0]); i++) {
            strcpy(&buffer, &argv[2]);                          //Coloco esto aquí y comento la linea antes de este if solo para comprobar la funcionalidad de write_f y read_f
            if (mi_write_f(ninodo, &buffer, arrayOffsets[i], sizeof(buffer))==-1) {
                perror("Error al intentar reservar un inodo." 
                "Función -> escribir.c - main()");
                exit(-1);
            }

            /*COMPROBACIONES DE CORRECTESA*/
            memset(&buffer, 0, sizeof(buffer));

            mi_read_f(ninodo, &buffer, arrayOffsets[i], sizeof(buffer));
            printf("Offset: %u\n", arrayOffsets[i]);
            write(1, &buffer, sizeof(buffer));
            puts("\n");
        }    

        //Mostrar datos del inodo escrito. 
        if (mi_stat_f(ninodo, &STAT) < 0) {
            perror("Error: mi_stat_f fallido." 
                "Función -> escribir.c - main()");
            exit(-1);
        }

        //Imprimir resultados
        printf("Tamaño en bytes lógico del inodo: %d\n", STAT.tamEnBytesLog);
        printf("Número de bloques ocupados: %d\n", STAT.numBloquesOcupados);

    } else {

        //Escribir cada offset en un inodo diferente
        for (int i = 0; i < sizeof(arrayOffsets)/sizeof(arrayOffsets[0]); i++) {
            //Reservar inodo
            ninodo = reservar_inodo('f', 6);

            //Mostrar el número de inodo por pantalla. 
            printf("El número de inodo reservado es: %d\n", ninodo);

            //Escribir en offset correspondiente a inodo
            if (mi_write_f(ninodo, &buffer, arrayOffsets[i], sizeof(buffer))<0){
                perror("Error al intentar reservar un inodo." 
                "Función -> escribir.c - main()");
                exit(-1);
            }

            /*COMPROBACIONES DE CORRECTESA*/
            memset(&buffer, 0, sizeof(buffer));

            mi_read_f(ninodo, &buffer, arrayOffsets[i], sizeof(buffer));
            printf("Offset: %u\n", arrayOffsets[i]);
            write(1, &buffer, sizeof(buffer));
            puts("\n");

            //Mostrar datos del inodo escrito
            if (mi_stat_f(ninodo, &STAT)<0) {
                perror("Error: mi_stat_f fallido." 
                "Función -> escribir.c - main()");
                exit(-1);
            }

            //Imprimir resultados
            printf("Tamaño en bytes lógico del inodo: %d\n", STAT.tamEnBytesLog);
            printf("Número de bloques ocupados: %d\n", STAT.numBloquesOcupados);
        }

    }

    //Desmontar dispositivo virtual. 
    if (bumount()==-1) {
        perror("Error: no se ha podido desmontar el dispositivo virtual.\n");
        exit(-1);
    }

    return 0;
=======
int main(int argc, char **argv) {

    

>>>>>>> Se crean los archivos necesarios y se añaden descripciones
}
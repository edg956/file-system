#include <stdio.h>
#include "ficheros.h"
#include "bloques.h"

int main(int argc, char **argv) {

    bmount(argv[1]);

    unsigned char buffer[strlen(argv[2])];
    strcpy((char *) buffer, argv[2]);
    char buffer2[strlen(argv[2])];

    int ninodo = reservar_inodo('d', 6);

    printf("ninodo: %i\n", ninodo);
    printf("Mensaje a escribir: %s (%li chars)\n", buffer, strlen(argv[2]));
    puts("Escribiendo...");
    mi_write_f(ninodo, buffer, 0, strlen(argv[2]));

    memset(buffer, 0, strlen(argv[2]));

    puts("\nLeyendo...\n");
    int leidos = mi_read_f(ninodo, &buffer, 0, strlen(argv[2]));
    
    write(1, buffer, leidos);
    printf("\nbytes leidos: %i\n", leidos);

    bumount();

    return 0;
}
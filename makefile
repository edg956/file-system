CC=gcc
CFLAGS=-c -g -Wall -std=gnu99
#LDFLAGS=-pthread

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
SOURCES=bloques.c ficheros_basico.c mi_mkfs.c leer_sf.c prueba.c liberar.c reservar.c#todos los .c
LIBRARIES=bloques.o ficheros_basico.o #todos los .o de la biblioteca del SF
INCLUDES=bloques.h ficheros_basico.h #todos los .h
PROGRAMS=mi_mkfs leer_sf prueba liberar reservar
=======
=======
>>>>>>> Se crea ficheros.h
=======
>>>>>>> Se crea ficheros.h
=======
>>>>>>> Se crea ficheros.h
SOURCES=bloques.c ficheros_basico.c mi_mkfs.c leer_sf.c ficheros.c #todos los .c
LIBRARIES=bloques.o ficheros_basico.o ficheros.o #todos los .o de la biblioteca del SF
INCLUDES=bloques.h ficheros_basico.h ficheros.h #todos los .h
PROGRAMS=mi_mkfs leer_sf
>>>>>>> Se crea ficheros.h
OBJS=$(SOURCES:.c=.o)

all: $(OBJS) $(PROGRAMS)

$(PROGRAMS): $(LIBRARIES) $(INCLUDES)
	$(CC) $(LDFLAGS) $(LIBRARIES) $@.o -o $@

%.o: %.c $(INCLUDES)
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: clean
clean:
	rm -rf *.o *~ $(PROGRAMS)
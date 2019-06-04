ENTREGA 3 - SISTEMAS OPERATIVOS II
GRUPO: Los tres mosqueteros. 
PARTICIPANTES: 
	- Eugenio Doñaque
	- Nadal Llabrés
 	- Álvaro Pons
FECHA: 05/06/2019

MEJORAS REALIZADAS: 

1- Utilización de cachés de lectura para obtener todas las entradas del directorio en verificacion.c

2- Utilizacion de cachés de lectura para obtener todos los registros del fichero prueba.dat de cada proceso en verificacion.c.

OBSERVACIONES: 

1- Implementado control de semaforos (concurrencia) en la función "mi_touch". Originalmente, no considerada en el nivel 12.

SINTAXIS:

1- ./mi_mkfs <nombre_del_dispositivo> <número_de_bloques>

2- ./mi_mkdir <nombre_del_dispositivo> <permisos> </ruta_directorio>

3- ./mi_touch <nombre_del_dispositivo> <permisos> </ruta_fichero>

4- ./mi_cat <nombre_del_dispositivo> </ruta_fichero>

5- ./mi_rm <nombre_del_dispositivo> </ruta>

6- ./mi_stat <nombre_del_dispositivo> </ruta>

7- ./mi_chmod <nombre_del_dispositivo> <permisos> </ruta>

8- ./mi_escribir <nombre_del_dispositivo> </ruta_fichero> <texto> <offset>

9- ./mi_link <nombre_del_dispositivo> <permisos> </ruta/>

10- ./mi_ls <nombre_del_dispositivo> </ruta_directorio/>

11- ./leer_sf <nombre_del_dispositivo>

12- ./simulacion <nombre_del_dispositivo>

13- ./verificacion <nombre_del_dispositivo> </ruta_directorio_simulacion/>


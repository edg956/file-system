/*
    Descripción:
        Dada una cadena de caracteres camino (que comience por '/'), 
        separa su contenido en dos.
        
        Ejemplos:
            camino = "/dir1/dir2/fichero"
            inicial = "dir1" (devuelve DIRECTORIO)
            final = "/dir2/fichero"

    Funciones a las que llama:

        
    Funciones desde donde es llamado:


    Parámetros de entrada:
        + const char *camino
        + char *inicial
        + char *final
        + char *tipo 

    Parámetros de salida:
*/
int extraer_camino(const char *camino, char *inicial, char *final, char *tipo) {

}


/*
    Descripción:    
        Como su nombre indica, esta función nos buscará la entrada indicada, 
        entre las entradas del inodo correspondiente a su directorio padre.

    Funciones a las que llama:
        + Extraer camino
        
    Funciones desde donde es llamado:

    Parámetros de entrada:
        + const char *camino_parcial
        + unsigned int *p_inodo_dir
        + unsigned int *p_inodo
        + unsigned int *p_entrada
        + char reservar
        + unsigned char permisos

    Parámetros de salida:

*/
int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo_dir, unsigned int *p_inodo, unsigned int *p_entrada, char reservar, unsigned char permisos) {

}
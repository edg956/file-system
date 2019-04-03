#include <stdio.h>
#include <string.h>


int prueba2 (char *buffer){
    printf("Buffer sin &: %p\n", buffer);
    printf("Buffer con &: %p\n", &buffer); 
    printf("Buffer con &: %p\n", &*buffer); 
    printf("Buffer sin &[0]: %d\n", buffer[0]);
    printf("Buffer con &[1]: %p\n", &buffer[1]); 
    printf("Buffer con &[0]+1: %p\n", &buffer[0]+1);
    printf("Buffer sin &[0]+1: %d\n", buffer[0]+1);
    return 0;
 }

int main() {

    char buffer[100]; 
    prueba2(buffer);
    puts("MAIN");
    printf("Buffer sin &: %p\n", buffer);
    printf("Buffer con &: %p\n", &buffer); 
    printf("Buffer con &: %p\n", &*buffer); 

    printf("Buffer sin &[0]: %d\n", buffer[0]);
    printf("Buffer con &[1]: %p\n", &buffer[1]); 
    printf("Buffer con &[0]+1: %p\n", &buffer[0]+1);
    printf("Buffer sin &[0]+1: %d\n", buffer[0]+1);

    char *str = {"hey"};
    char buf[4];

    strcpy(buf, str);

    printf("STR: %s Comparación: %i\n", buf, strcmp(buf, str));
    

}


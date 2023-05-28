#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <fcntl.h> 

int main(int argc, char **argv) {
    printf("Il valore di BUFSIZ è %d\n",BUFSIZ);
    exit(0);
}
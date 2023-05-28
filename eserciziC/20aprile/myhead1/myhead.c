#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char **argv) {

    //controllo sul numero di parametri
    if (argc != 2) {
        printf("Errore: numero di parametri passato errato\n");
        exit(1);
    }

    //controlliamo che ci sia il simobolo -
    if (argv[1][0] != '-') {
        printf("Errore: necessario il simbolo optzionale\n");
        exit(2);
    }

    //controlliamo che il primo parametro sia un numero
    int N = atoi(&argv[1][1]);
    if (N < 0) {
        printf("Errore: parametro passato non è strettamente positivo");
        exit(3);
    }
    exit(0);
    
}
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
int main(int argc, char **argv) {
    //controllo sul numero di parametri
    printf("Controlliamo il numero di parametri\n");
    if (argc != 4) {
        printf("Errore: nuemero di parametri passato errato");
        exit(1);

    }
    printf("Numero di parametri corretto\n");
    printf("Controllo che il primo parametro sia un nome di un file\n");
    //controllo che il primo parametro sia un nome di un file
    if ((open(argv[1],O_RDONLY)) == -1) {
        printf("Errore: parametro passsato %s non è un file",argv[1]);
        exit(2);
    }
    printf("Primo parametro corretto\n");
    printf("Controllo che il secondo parametro sia un numero\n");

    //controllo che il secondo parametro sia un numero 
    if ((atoi(argv[2])) <=0) {
        printf("Errore: parametro passato %s non è un numero",argv[2]);
        exit(3);
    }
    printf("Secondo parametro corretto\n");
    printf("Controllo che il terzo parametro sia un singolo carattere\n");
    //controlliamo che il terzo parametro sia un singolo carattere
    if (strlen(argv[3]) != 1) {
        printf("Errore:parametro passato %s non è un singolo carattere",argv[3]);
        exit(4);
    }
    printf("Terzo carattere corretto");
    exit(0);
}

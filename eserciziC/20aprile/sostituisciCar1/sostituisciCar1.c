#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>


int main(int argc, char **argv) {
    int fd;
    int nl;
    char letto;
    //controllo il numero di parametri 
    if (argc != 4) {
        printf("Errore: numero di parametri passato errato");
        exit(1);
    }

    //controllo che il primo parametro sia un nome di un file
    if ((fd = open(argv[1],O_RDWR)) < 0) {
        printf("Errore: primo parametro passato non è un nome di un file");
        exit(2);
    }

    //controllo che il secondo parametro sia un singolo carattere
    if ((nl = strlen(argv[2])) != 1) {
        printf("Errore: secondo parametro passato non è un singolo carattere");
        exit(3);
    }

    //controllo che il terzo parametro sia un singolo carattere
    if ((nl = strlen(argv[3])) != 1) {
        printf("Errore: terzo parametro passato non è un singolo carattere");
        exit(4);
    }
    
    while(read(fd,&letto,1) > 0) {
        if (letto == argv[2][0]) {
            //facciamo andare indietro il pointer di uno e poi scriviamo 
            lseek(fd, -1L,1);
            //scriviamo il carattere change
            char change = argv[3][0];
            write(fd,&change,1);
        }
    }
    exit(0);

}
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char **argv) {

    //controlliamo il numero di parametri
    if (argc != 3) {
        printf("Errore: numero di parametri passati errato");
        exit(1);
    }
    int fd = 0;
    //controlliamo che il primo parametro sia un nome di un file
    if ( (fd = open(argv[1],O_RDONLY)) == -1 ) {
        printf("Errore: primo parametro passato non è un nome di un file");
        exit(2);
    }
    //controlliamo che il secondo parametro sia un singolo carattere
    if (strlen(argv[2]) != 1) {
        printf("Errore: secondo parametro passato non è un singolo carattere");
        exit(3);
    }
    //visualizziamo il nome dell'eseguibile
    printf("Nome dell'eseguibile: %s\n",argv[0]);

    char buffer[BUFSIZ];
    long int cont = 0;
    while((read(fd,buffer,1)) > 0) {
        //controlliamo l'occorenza del carattere 
        if (buffer[0] == *argv[2]) {
            cont++;
        }
    }
    //stampiamo il numero di occorenze del file
    printf("Numero di occorrenze del carattere %s : %ld",argv[2],cont);
    exit(0);
}
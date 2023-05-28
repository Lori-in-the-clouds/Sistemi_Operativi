#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h> 

int main(int argc, char **argv) {
    int fd;
    int n;
    char *buffer;
    
    //controllo sul numero di parametri
    if (argc != 3) {
        printf("Errore: numero di parametri passato errato");
        exit(1);
    }

    //cotrolliamo che il primo parametro sia un nome di un file
    if ((fd = open(argv[1],O_RDONLY)) < 0) {
        printf("Errore: primo parametro passato non è un nome di un file");
        exit(2);
    }

    //controlliamo che il secondo parametro sia un numero strettamente positivo
    n = atoi(argv[2]);
    if (n < 0) {
        printf("Errore: secondo parametro passato non è strettamente positivo");
        exit(3);
    }   
    
    int control = 1;
    int index =1;
    buffer = (char *)(malloc(256* sizeof(char)));

    while(index < n + 1) {
        int j=0;
        if (control == 0) {
              printf("Riga non trovata");
            exit(4);
        }
        while ((control =read(fd,&(buffer[j]),1)) != 0) {
            if (buffer[j]=='\n' || j==255) {
                index++;
                break;
            }
            j++;
        }
       
    }
    //stampiam il buffer
    printf("Riga selezionata: %s",buffer);
    exit(0);
}
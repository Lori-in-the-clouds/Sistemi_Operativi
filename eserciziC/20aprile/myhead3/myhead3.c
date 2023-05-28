#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char **argv) {
    int N;
    int fd;
    
    //controllo sul numero di parametri
    if (argc > 3) {
        printf("Errore: numero di parametri passato errato\n");
        exit(1);
    }

    if (argc >= 2) {
        //controlliamo che ci sia il simobolo -
        if (argv[1][0] != '-') {
            printf("Errore: necessario il simbolo optzionale\n");
            exit(2);
        }

        //controlliamo che il primo parametro sia un numero
        N = atoi(&argv[1][1]);
        if (N < 0) {
            printf("Errore: parametro passato non è strettamente positivo");
            exit(3);
        }
    }
    if (argc == 1) {
        N=10;
    }
    //controlliamo che il terzo parametro sia un nome di un file
    if (argc == 3) {
        if ((fd = open(argv[2],O_RDONLY)) < 0) {
            printf("Errore: terzo parametro passato non è un nome di un file");
            exit(4);
        }
    }

   
    int currentline=1;
    char tmp;
    if (argc <= 2) {
        while(read(0,&tmp,1)) {
            
            if (tmp == '\n') currentline++;
            write(1,&tmp,1);
            if (currentline > N) break;
        }
    } else {

         while(read(fd,&tmp,1)) {
            
            if (tmp == '\n') currentline++;
            write(1,&tmp,1);
            if (currentline > N) break;
        }


    }

    exit(0);
    
}
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <fcntl.h> 
#include <string.h>
#include <time.h>
#include <sys/wait.h>

int main(int argc,char **argv) {
    int fd;
    int i;
    int pid;
    int N = argc -2;
    int pidFiglio,status,ritorno; //per il wait e valore di ritorno figli
    
    //controllo il numero di parametri 
    if (argc < 4) {
        printf("Errore: numero di parametri inseriti errato");
        exit(1);
    }
    
    //controllo l'ultimo parametro
    if (strlen(argv[argc -1]) != 1) {
        printf("Errore: l'ultimo parametro non è un singolo carattere\n");
        exit(3);
    }

    //creiamo i processi figli
    for(i=0;i<N;i++) {
        if ((pid = fork()) < 0) {
            printf("Errore nella fork\n");
            exit(4);
        }
        
        //codice del figlio
        if (pid == 0) {

          //apriamo il file 
            if ((fd = open(argv[i+1],O_RDONLY)) < 0) {
                exit(-1); //verrà intrerpretato dal padre come 255
            }
            char buff;
            int cont=0;
            while(read(fd,&buff,1) > 0) {
                if (buff == *argv[argc -1]) {
                    cont++;
                }
            }
            exit(cont);
        }
    }

    //padre
    for (int i=0;i<N;i++) {
        pidFiglio= wait(&status);
        if (pidFiglio < 0) {
            printf("Errore in wait\n");
            exit(4);
        }
        if ((status & 0xFF) != 0) {
            printf("Figlio con pid %d terminato in modo anomalo\n",pidFiglio);
        } else {
            ritorno = ((int)(status >> 8) & 0xFF);
            if (ritorno == 255) {
                printf("Il figlio con PID: %d non è riuscito ad aprire il file\n",pidFiglio);
                
            } else {
                printf("Il figlio con PID: %d ha riportato il seguente valore: %d\n",pidFiglio,ritorno);
            }

        }
    }

}
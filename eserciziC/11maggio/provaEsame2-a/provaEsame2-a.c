#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

typedef int pipe_t[2];

int main(int argc, char **argv) {
    int pid;
    int status,ritorno;
    int N = argc - 2; /*numero di parametri*/
    int fd; 
    int j,k;
    long  int count;
    pipe_t *piped;
    //controllo sul numero di parametri 
    if (argc <= 3) {
        printf("Errore: numero di parametri errato\n");
        exit(1);
    }

    

   

    //controlliamo che i restanti parametri siano sincoli caratteri
    for (j =1;j < N; j++) {
        int lenght = strlen(argv[j+1]);
        if (lenght != 1) {
            printf("Errore: parametro passato %s non è un singolo carattere\n",argv[j+1]);
            exit(3);
        }
    }

    /*allocazione array di N pipe*/
    piped = (pipe_t *)malloc(N*sizeof(pipe_t));
    if (piped == NULL)  {
        printf("Errore nella allocazione della memoria\n");
        exit(4);
    }
    
    /*Creazione delle N pipe per la comunicazione figli-padre*/
    for (j=0;j<N;j++) {
        if (pipe(piped[j]) < 0) {
            printf("Errore nella creazione della pipe\n");
            exit(5);
        }
    }

    /*Ciclo di generazione dei figli*/
    for(j=0;j<N;j++) {
        
        if ((pid = fork()) < 0) {
            printf("Errore nella fork\n");
            exit(6);
        }

        /*processo figlio*/
        if (pid == 0) {
            
            /*Chiusura delle pipe non usat nella comunicazione con il padre*/
            for(k=0;k<N;k++) {
                close(piped[k][0]);
                if (k != j) close(piped[k][1]);
            }
            
            //facciamo quello che deve fare il figlio
             count =0;
            
            //controlliamo che il primo parametro sia un nome di un file
            if ((fd = open(argv[1],O_RDONLY)) < 0) {
                printf("Errore sul primo parametro, non è un nome di un file\n");
                exit(2);
            }
            char ch;
            //leggiamo i file carattere per carattere
            while(read(fd,&ch,1)) {
                if (ch ==  (*argv[j+2])) { //attenzione
                    count++;
                }
            }

            write(piped[j][1],&count,sizeof(long int));
            exit(*argv[j+2]);
        }


    }   

    /*codice padre*/
    
    /*il padre chiude i lati delle pipe che non usa*/
    for (j=0;j<N;j++) {
        close(piped[j][1]);
    }

    /*il padre recupera le informazioni dai figli in ordine inverso di indice*/
    for (j=N-1; j>=0; j--) {
        /*il padre recupera tutti i valori interi dai figli*/
        read(piped[j][0],&count,sizeof(long int));
        printf("Il figlio di indice %d ha comunicato il valore %ld come numero di caratteri di %s, per il file %s\n",j,count,argv[j+2],argv[1]);
    }

    /*Il padre aspetta i figli*/
    for (j=0;j<N;j++) {
        
        if ((pid = wait(&status)) < 0) {
            printf("Errore nella wait\n");
            exit(7);
        }

        if ((status & 0xFF) != 0) {
            printf("Figlio con pid %d terminato in modo anomalo\n",pid);
            exit(8);
        }
        else {
            ritorno = (int)((status >> 8) & 0xFF);
            printf("Il figlio con pid=%d ha ritornato %c\n",pid,ritorno);
        }
    }
    exit(0);
}
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
    int N = argc -1;
    int j,i,k;
    char *ch = malloc(255*sizeof(char));
    int integer;
    pipe_t *piped;
    pipe_t p; /*creazione di una singola pipe tra figlio e nipote*/
    
    /*controllo il numero di parametri*/
   if (argc < 3) {
    printf("Errore, numero di parametri passato errato\n");
    exit(1);
   } 
   
   /*allocazione dell'array di N pipe descriptors*/
    piped = (pipe_t*)malloc(N*sizeof(pipe_t));
    if (piped == NULL) {
        printf("Errore nella allocazione della memoria\n");
        exit(2);
    }
   
    /*Creazione delle N pipe figli-padre*/
    for (j=0; j<N; j++) {
        if (pipe(piped[j]) < 0) {
            printf("Errore nella creazione della pipe\n");
            exit(3);
        }
    }

    /*ciclo di generazione dei figli*/
    for (j=0;j<N;j++) {
        if ((pid = fork()) < 0) {
            printf("Errore nella fork\n");
            exit(4);
        }

        /*processo figlio*/
        if (pid == 0) {

            /*chiusura delle pipe non usate nella comunicazione con il padre*/
            for (k=0;k<N;k++) {
                close(piped[k][0]);
                if (k != j) close(piped[k][1]);
            }

            /*creiamo la pipe di comunicazione fra nipote e figlio*/
            if (pipe(p) < 0) {
                printf("Errore nella creazione della pipe fra figlio e nipote\n");
                exit(-1);
            }

            if ((pid = fork()) < 0) {
                printf("Errore nella fork di creazione del nipote\n");
                exit(-2);
            }

            /*processo nipote*/
            if (pid == 0) {
                /*chiusura della pipe rimasta aperta di comunicazione fra figlio-padre che il nipote non usa*/
                close(piped[j][1]);
            
                /*ridirezione dello standard input*/
                close(0);
                if (open(argv[j+1],O_RDONLY) < 0) {
                    printf("Errore nella open del file %s\n",argv[j+1]);
                    exit(-3);
                }

                /*ridirezione dello standar output verso il pipe*/
                close(1);
                dup(p[1]);

                /*il nipote chiude entrambi i lati della pipe*/
                close(p[0]);
                close(p[1]);
                
                /*ridirezione dello standard error su /dev/null*/
                close(2);
                open("/dev/null",O_WRONLY);

                /*il nipote diventa il comando wc*/
                execlp("wc","wc","-l",(char *)0);

                /*qui non dovrebbe mai andarci*/
                exit(-4);

            }

            /*parte del codice figlio*/
            /*il figlio deve chiudere la pipe di comunicazione con il nipote*/
            close(p[1]);

            /*il figlio legge dalla pipe e poi scrive nella pipe verso il padre*/
            i=0;
            while(read(p[0],&ch[i],1)) {
                if (ch[i] == '\n') {
                    ch[i] = '\0';
                    break;
                }
                i++;
            }
            int integer = atoi(ch);
            
            /*scriviamo sulla pipe*/
            write(piped[j][1],&integer,sizeof(int));
            
            /*il figlio aspetta il nipote*/
            if ((pid = wait(&status)) < 0 ) {
                printf("Errore nella wait\n");
                exit(-5);
            }
            if ((status & 0xFF) != 0) {
                printf("Errore processo nipote terminato in modo anomalo\n");
                exit(-6);
            }
            ritorno = (int)((status >> 8) & 0xFF);
            
            if (ritorno != 0) {
                printf("Errore nel wc -l dei file %s",argv[j+1]);
            }
            exit(integer);

        }
    } 
    /*codice del padre*/
    int tot = 0;
    /*il padre chiude i lati della pipe che non usa*/
    for (j=0;j<N;j++) {
        close(piped[j][1]);
    }
    
    /*riotteniamo i dati dalla pipe*/
    for(j=0;j<N;j++) {
        read(piped[j][0],&integer,sizeof(int));
        tot+= integer;
    }

    /*stapiamo il numero totale di righe trovate*/
    printf("Totale di righe trovate:%d\n",tot);

    /*il padre aspetta i figli*/
    for (j=0;j<N;j++) {
        if ((pid = wait(&status)) < 0) {
            printf("Errore in wait\n");
            exit(5);
        }

        if ((status & 0xFF) != 0) {
            printf("Figlio con pid %d terminato in modo anomalo\n",pid);
        }
        ritorno = (int)((status >> 8) & 0xFF);
        printf("Il figlio con pid=%d ha ritornato %d\n",pid,ritorno);
    }
    exit(0);
}
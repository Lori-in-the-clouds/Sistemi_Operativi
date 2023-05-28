#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>

typedef int pipe_t[2];

int main(int argc,char **argv) {
    int Q;  /*numero di processi figli*/
    int q;  /*indice dei processi figli*/
    int L;  /*lunghezza in linee de file F*/
    int j;
    int status,ritorno,pid,pidFiglio;
    int fd;
    char ch;
    int nr,nw;
    pipe_t *pipes;
    char ok;
    

    /*controllo sul numero di parametri*/
    if (argc < 5) {
        printf("Errore numero di parametri, dato che argc=%d\n",argc);
        exit(1);
    }

    /*controllo sul secondo parametro*/
    L = atoi(argv[2]);
    if (L < 0) {
        printf("Errore secondo parametro, deve essere strettamente positivo\n");
        exit(2);
    }

    /*controllo dei Q parametri*/
    Q = argc - 3;
    for (j=3;j<argc;j++) {
        if (strlen(argv[j]) != 1) {
            printf("Errore, parametro passato %s non è un singolo carattere\n",argv[j]);
            exit(3);
        }
    }
   
   /*allocazioen pipe*/
   if ((pipes=(pipe_t*)malloc(Q*sizeof(pipe_t))) == NULL) {
        printf("Errore allocazione pipe\n");
        exit(4);
   }

   /*creazione pipe*/
   for (q=0;q<Q;q++) {
        if (pipe(pipes[q]) < 0) {
            printf("Errore nella creazione pipe\n");
            exit(5);
        }
   }

   /*creazione figli*/
   for (q=0;q<Q;q++) {

        if ((pid = fork()) < 0) {
            printf("Errore nella fork\n");
            exit(6);
        }

        if (pid == 0) {
            /*codice figlio*/

            /*chiudiamo le pipe non utilizzate*/
            for (j=0;j<Q;j++) {
                if (j != q) close(pipes[j][0]);
                if (j != (q+1) % Q) close(pipes[j][1]);
            }

            /*apriamo il file*/
            if ((fd = open(argv[1],O_RDONLY)) < 0) {
                printf("Errore nella apertura del file\n");
                exit(-1);
            }

            j=0; /*numero di occorenze per ogni riga*/
            while(read(fd,&ch,sizeof(ch))) {

                /*se abbiamo trovato un carettere*/
                if (ch == *argv[q+3]) {
                    j++;
                }

                /*quando arriviammo a fine linea*/
                if (ch == '\n') {
                    
                    /*riceviamo l'ok dal processo prima dell'anello*/
                    nr = read(pipes[q][0],&ok,sizeof(ok));
                    
                    /*controlliamo il risultato della lettura della pipe*/
                    if (nr != sizeof(ok)) {
                        printf("Il figlio %d ha letto un numero di byte sbagliati\n",q);
                        exit(-1);
                    }

                    printf("Il figlio con indice %d e pid %d ha letto %d caratteri %c nella linea corrente\n",q,getpid(),j,*argv[q+3]);
                    
                    /*mandiamo l'ok al prossimo processo dell'anello*/
                    nw = write(pipes[(q+1)%Q][1],&ok,sizeof(ok));

                    /*controlliamo il risultato della scrittura della pipe*/
                    if (nw != sizeof(ok)) {
                        printf("Il figlio %d ha scritto un numero di byte sbagliati\n",q);
                        exit(-1);
                    }
                    ritorno = j;
                    j=0;
                }
            }
            
            exit(ritorno);
        }
   }

   /*codice padre*/

   /*chiusura di tutte le pipe che non usa, tranne la pipe[0]*/
   for (q=1;q<Q;q++) {
        close(pipes[q][0]);
        close(pipes[q][1]);
   }

   /*si deve mandare l'ok al primo figlio*/
   nw = write(pipes[0][1],&ok,sizeof(ok));
   
   /*controlliamo il risultato della scrittura della pipe*/
    if (nw != sizeof(ok)) {                    
        printf("Il figlio %d ha scritto un numero di byte sbagliati\n",q);
        exit(-1);
    }

    /*ora possiamo scrivere il lato di scrittura, MA NON QUELLO DI LETTURA*/
    close(pipes[0][1]);

    /*il padre aspetta i figli*/
    for (q=0;q<Q;q++) {

        if ((pidFiglio = wait(&status)) < 0) {
            printf("Errore in wait\n");
            exit(7);
        }

        if ((status & 0xFF) != 0) {
            printf("Figlio con pid %d terminato in modo anomalo\n",pidFiglio);
        } else {
            ritorno = (int)((status >> 8) & 0xFF);
            printf("Il figlio con pid=%d ha ritornato %d(se 255 problemi)\n",pidFiglio,ritorno);
        }

    }
    exit(0);
}
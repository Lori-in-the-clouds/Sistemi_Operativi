#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

typedef int pipe_t[2];
typedef struct {
    int c1;
    int c2;
    char c3[250];
} structer;

int main(int argc, char **argv) {
    int pid;
    int N;
    int status,ritorno;
    int j,k;
    int fd;
    pipe_t *piped;
    pipe_t p; /*creazione di una singola pipe tra figlio e nipote*/
    structer C;
    char ch;
    
    /*controllo sul numero di parametri*/
    if (argc < 4) {
        printf("Errore nel numero di parametri, dato che argc=%d\n",argc);
        exit(1);
    }

    /*calcoliamo il numero di file passati*/
    N = argc -1;

    /*allocazione dell'array di N pipe descriptors*/
    piped = (pipe_t*)malloc(N*sizeof(pipe_t));
    if (piped == NULL) {
        printf("Errore nella creazione della pipe\n");
        exit(2);
    }

    /*creazione delle N pipe figli-padre*/
    for (j=0;j<N;j++) {
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

        if (pid == 0) {
            /*codice figlio*/

            /*chiusura delle pipe non usate nella comunicazione con il padre*/
            for (k=0;k<N;k++) {
                close(piped[k][0]);
                if (k != j) close(piped[k][1]);
            }

            /*prima creiamo la pipe di comunicazione tra nipote e figlio*/
            if (pipe(p) < 0) {
                printf("Errore nella comunicazione della pipe fra figlio e nipote\n");
                exit(-1);
            }

            if ((pid = fork()) < 0) {
                printf("Errore nella fork di creazione del nipote\n");
                exit(-2);
            }

            if (pid == 0) {
                /*codice nipote*/

                /*chiusura della pipe rimasta aperta di comunicazione fra figlio-padre che il nipote non usa*/
                close(piped[j][1]);

                /*apriamo il file*/
                if ((fd = open(argv[j+1],O_RDONLY)) < 0) {
                    printf("Errore nella open del file %s\n",argv[j+1]);
                    exit(-3);
                }

                /*ricaviamo la prima riga del file*/
            
                int s = 0;
               
                while(read(fd,&ch,1)) {
                    if (ch == '\n') {
                        C.c3[s]='\0';
                        C.c2 = s;
                        break;
                    }
                    C.c3[s]=ch;
                    s++;
                }
                C.c1 = getpid();
                close(1);
                dup(p[1]);
                
                /*il nipote chiude entrambi i lati dell pipe*/
                close(p[0]);
                close(p[1]);

                write(1,&C,sizeof(C));

                /*ridirezione dello standard output su /dev/null */
                close(1);
                open("/dev/null",O_WRONLY);

                /*ridirezione dello standard errore su /dev/null*/
                close(2);
                open("/dev/null",O_WRONLY);

                /*utilizziamo il comando sort e ordiniamo il file*/
                execlp("sort","sort",argv[j+1],(char *)0);

                /*QUI NON CI DOVREBBE MAI ANDARE*/
                exit(-4);
            }

            /*codice figlio*/

            /*ogni figlio deve chiudere il lato che non usa della pipe di comunicazione con il nipote*/
            close(p[1]);

            /*il figlio legge la pipe*/

            while(read(p[0],&C,sizeof(C))) {
                /*il figlio comunica al padre*/
              
                write(piped[j][1],&C,sizeof(structer));
            }

            /*il figlio deve aspettare il nipter per resitutire il valore al padre*/
            if ((pid = wait(&status)) < 0) {
                printf("Errore in wait\n");
                exit(-5);
            }

            if ((status & 0xFF) != 0) {
                printf("Nipote con pid %d terminato in modo anomalo\n",pid);
            }
            ritorno = (int)((status >> 8) & 0xFF);
            if (ritorno != 0) {
                printf("Errore nella exev");
            }
            exit(C.c2 -1);

        }
    }

    /*codice padre*/

    /*il padre chiude i lati delle pipe che non usa*/
    for (j=0;j<N;j++) {
        close(piped[j][1]);
    }

    /*il padre recupera le informazioni dei figli*/
    for (j=0;j<N;j++) {
        
        /*il padre recupera tutti i valori interi dai figli*/
        read(piped[j][0],&C,sizeof(C));
        printf("Il figlio di indice %d ha comunicato:\n",j);
        printf("C.c1 = %d\n",C.c1);
        printf("C.c2 = %d\n",C.c2);
        printf("C.c3 = %s\n",C.c3);
    }

    /*il padre aspetta i figli*/
    for (j=0;j<N;j++) {
        if ((pid = wait(&status)) < 0) {
            printf("Errore nella wait\n");
            exit(5);
        }

        if ((status & 0xFF) != 0) {
            printf("Figlio con pid %d terminato in modo anomalo\n",pid);
        }
        else {
            ritorno = (int)((status >> 8) & 0xFF);
                printf("Il figlio con pid=%d ha ritornato %d\n",pid,ritorno);        
        }

    }
    exit(0);
}
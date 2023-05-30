#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
typedef int pipe_t[2];
typedef struct {
    char c; /*carattere cercato originale*/
    long int occ; /*numero di occorrenze di c*/
    char cx; /*carattere cercato complementare*/
    long int occx; /*numero di occorrenze di cx*/
} structer;


int main(int argc,char **argv) {
    /*--------------*variabili locali*--------------*/
    int N;	/*numero di processi figli*/
    int i,j,k;
    char ch;
    int fd;	/*file descriptor per apertura file*/
    int pid;
    int pidFiglio,status,ritorno;	/*per fork e wait*/
    pipe_t *piped;
    structer A;
    /*----------------------------------------------*/    

    /*controllo sul numero di parametri*/
    if (argc < 4) {
        printf("Errore numero di parametri, dato che argc=%d\n",argc);
        exit(1);
    }

    N = argc -2;

    /*Controllo che  sia un singolo carattere alfabetico*/
    if (strlen(argv[argc-1]) != 1 && ( (argv[argc -1][0] >= 65 && argv[argc-1][0] <= 90) || (argv[argc -1][0] >= 97 && argv[argc-1][0] <= 122) )) {
        printf("Errore: parametro passato %s non è un singolo carettere alfabetico\n",argv[argc-1]);
        exit(2);
    }

    A.c = argv[argc-1][0];
    if (islower(A.c)) {
        A.cx = A.c -32;
    } else A.cx = A.c + 32;

    /*allocazione dell'array di N pipe descriptors*/
    piped= (pipe_t*)malloc(N*sizeof(pipe_t));
    if (piped == NULL) {
        printf("Errore: problemi nell'allocazione di memoria per la pipe\n");
        exit(3);
    }

    for(j=0;j<N;j++) {
        if(pipe(piped[j]) < 0) {
            printf("Errore: problemi nella creazione della pipe\n");
            exit(4);
        }
    }

    printf("DEBUG-Sono il processo padre con pid %d e sto per creare %d figli \n",getpid(),N);
    for(i=0;i<N;i++) {
    
        if((pid = fork()) < 0) {
            printf("Errore nella fork\n");
            exit(5);
        }
        if(pid == 0) {
            /*codice figlio*/
            
            /*chiudiamo pipe che non utilizziamo*/
            for (k=0;k<N;k++) {
                close(piped[k][0]);
                if(i != k) close(piped[k][1]);
            }

            /*apro il file*/
            if ((fd = open(argv[i+1],O_RDONLY))<0) {
                 printf("Errore: impossibile aprire il file %s\n",argv[i+1]);
                exit(-1);
            }
            A.occ = 0L;
            A.occx = 0L;
            while(read(fd,&ch,1)) {
                if (ch == A.c) {
                    /*abbiamo trovato un'occorrenza del carattere c*/
                    A.occ++;
                }
                if (ch == A.cx) {
                    /*abbiao trovato un'occorrenza del carattere cx*/
                    A.occx++;
                }
            }

            /*scriviamo al padre la struttura ottenuta*/
            write(piped[i][1],&A,sizeof(A));
            
            if (A.occ > A.occx) {
                exit(1);
            } else exit(0);

        }
    }
    /*codice padre*/
    /*chiudiamo le pipe che non utilizziamo*/
    for (j=0;j<N;j++) {
        close(piped[j][1]);
    }

    for (i=0;i<N;i++) {
        read(piped[i][0],&A,sizeof(A));
        printf("Il figlio di indice %d ha riportato la seguente struttura:\n",i);
        printf("c->%c occ->%ld cx->%c occx->%ld\n",A.c,A.occ,A.cx,A.occx);
    }

    /*Il padre aspetta tutti i processi figli*/
    for(j=0;j<N;j++) {
        if((pidFiglio = wait(&status)) < 0) {
            printf("Errore wait\n");
            exit(6);
        }
        if((status & 0xFF) != 0) {
            printf("Figlio con pid %d terminato in modo anomalo\n",pidFiglio);
             exit(7);
        } else {
             ritorno = (int)((status >> 8) & 0xFF);
             printf("Il figlio con pid = %d ha ritornato %d (se 255 problemi!)\n",pidFiglio,ritorno);
         }
    }
    exit(0);

}
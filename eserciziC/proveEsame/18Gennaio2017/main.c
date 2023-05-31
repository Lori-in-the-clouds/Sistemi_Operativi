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
    int c1; /*contiene l'indice d'ordine dei processi*/
    int c2; /*contiene il numero corrispondente al carattere numerico*/
} structer;

int main(int argc,char **argv) {
    /*--------------*variabili locali*--------------*/
    int N;	/*numero di processi figli*/
    int i,j;
    char ch;
    int fd;	/*file descriptor per apertura file*/
    int nr,nw;	/*per controllare lettura e scrittura pipe*/
    int *pid;
    int pidFiglio,status,ritorno;	/*per fork e wait*/
    pipe_t *piped;
    structer *A;
    /*----------------------------------------------*/

    /*controllo sul numero di parametri*/
    if (argc < 2) {
        printf("Errore numero di parametri, dato che argc=%d\n",argc);
        exit(1);
    }
    N = argc -1;

    /*allochiamo l'array di pid*/
    pid = (int*)malloc(N*sizeof(int));
    if (pid == NULL) {
        printf("Errore nella allocazione della pipe\n");
        exit(2);
    }
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
    
        if((pid[i] = fork()) < 0) {
            printf("Errore nella fork\n");
            exit(5);
        }
        if(pid[i] == 0) {
            /*codice figlio*/
            /*chiusura pipes inutilizzate*/
            for (j=0;j<N;j++) {
                if (j!=i)
                    close(piped[j][1]);
                if ((i==0) || (j != i-1))
                    close(piped[j][0]);
            }

            /*apro il file*/
            if ((fd = open(argv[i+1],O_RDONLY))<0) {
                 printf("Errore: impossibile aprire il file %s\n",argv[i+1]);
                exit(-1);
            }

            /*allocazione dell'array di strutture specifico di questo figlio*/
            if ((A = (structer*)malloc((i+1)*sizeof(structer))) == NULL) {
                printf("Errore nella allocazione della memoria\n");
                exit(6);
            }

            while(read(fd,&ch,1)) {
                
                if (isdigit(ch)) {
                    /*trovata occorrenza carattere numerico*/

                    if (i!=0) {

                        /*leggiamo l'array proveniente dal figlio precedente*/
                        nr = read(piped[i-1][0],A,i*sizeof(A));
                        if( nr != sizeof(i*sizeof(A))) {
                            printf("Errore: il  ha letto un numero di byte sbagliati\n");
                            exit(7);
                        }
                        
                        A[i].c1 = i;
                        A[i].c2 = ch - '0';
                    
                    } else {
                        A[0].c1 = 0;
                        A[0].c2 = ch - '0';
                    }

                    /*scriviamo al figlio successivo*/
                    nw = write(piped[i][1],A,(i+1)*sizeof(A));
                    if( nw != (i+1)*sizeof(A)) {
                        printf("Errore:il   ha scritto un numero di byte sbagliati\n");
                        exit(8);
                    }
                    
                }
            }
            exit(A[i].c2 + '0');
        }
    }
    
    /*codice padre*/
    /*chiusura pipe: tutte meno l'ultima in lettura*/
    for (i=0;i<N;i++) {
        close(piped[i][1]);
        if (i != N - 1) close(piped[i][0]);
    }

    /*il padre alloca memoria per l'array di struct*/
    A = (structer*)malloc(N*sizeof(structer));
    if (A == NULL) {
        printf("Errore nella allocazione della memoria\n");
        exit(9);
    }

    /*leggiamo l'array di stuct inviata dal padre*/
    nr = read(piped[N-1][0],A,N*sizeof(A));
    if( nr != N*sizeof(A)) {
        printf("Errore: il  ha letto un numero di byte sbagliati\n");
        exit(10);
    }

    for (j=0;j<N;j++) {
        printf("Il figlio con pid %d ha ritornato la seguente struct:\n",pid[j]);
        printf("c1->%d c2->%d\n",A[j].c1,A[j].c2);
    }

    /*Il padre aspetta tutti i processi figli*/
    for(j=0;j<N;j++) {
        if((pidFiglio = wait(&status)) < 0) {
            printf("Errore wait\n");
            exit(11);
        }
        if((status & 0xFF) != 0) {
            printf("Figlio con pid %d terminato in modo anomalo\n",pidFiglio);
             exit(12);
        } else {
             ritorno = (int)((status >> 8) & 0xFF);
             printf("Il figlio con pid = %d ha ritornato %c (se 255 problemi!)\n",pidFiglio,ritorno);
         }
    }
    exit(0);

    
}
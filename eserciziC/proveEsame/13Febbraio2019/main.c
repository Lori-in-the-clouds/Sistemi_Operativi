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
    long int c1; /*numero massimo di linee calcolato dal processo*/
    int c2; /*contiene l'indice d'ordine che ha calcolato il massimo*/
} structer;

int main(int argc,char **argv) {
    /*--------------*variabili locali*--------------*/
    int N;	/*numero di processi figli*/
    int i,j,k;
    char ch;
    int fd;	/*file descriptor per apertura file*/
    int nr,nw;	/*per controllare lettura e scrittura pipe*/
    int *pid;
    int pidFiglio,status,ritorno;	/*per fork e wait*/
    pipe_t *piped;
    structer A;
    /*----------------------------------------------*/

    /*controllo sul numero di parametri*/
    if (argc < 2) {
        printf("Errore numero di parametri, dato che argc=%d\n",argc);
        exit(1);
    }
    N = argc-1;

    /*allocazione array di pid*/
    pid = (int*)malloc(N*sizeof(int));

    /*allocazione dell'array di N pipe descriptors*/
    piped= (pipe_t*)malloc(N*sizeof(pipe_t));
    if (piped == NULL) {
        printf("Errore: problemi nell'allocazione di memoria per la pipe\n");
        exit(2);
    }

    for(j=0;j<N;j++) {
        if(pipe(piped[j]) < 0) {
            printf("Errore: problemi nella creazione della pipe\n");
            exit(3);
        }
    }

    printf("DEBUG-Sono il processo padre con pid %d e sto per creare %d figli \n",getpid(),N);
    for(i=0;i<N;i++) {
    
        if((pid[i] = fork()) < 0) {
            printf("Errore nella fork\n");
            exit(-1);
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
            k = 0; /*contatore del numero di linee*/
            while(read(fd,&ch,1)) {
                if (ch == '\n') k++;
            }


            if (i != 0) {
                /*leggiamo dalla pipe precedente*/
                nr = read(piped[i-1][0],&A,sizeof(A));
                
                if( nr != sizeof(A)) {
                    printf("Errore: il  ha letto un numero di byte sbagliati\n");
                    exit(-1);
                }

                if (k > A.c1) {
                    A.c1 = k;
                    A.c2 = i;
                }
            } else {
                A.c1 = k;
                A.c2 = 0;
            }

            /*scriviamo sulla pipe successiva*/
            nw = write(piped[i][1],&A,sizeof(A));
            if( nw != sizeof(A)) {
                printf("Errore:il   ha scritto un numero di byte sbagliati\n");
                exit(-1);
            }
            exit(i);
        }
    }

    /*codice padre*/
    /*chiusura pipe: tutte meno l'ultima in lettura*/
    for (i=0;i<N;i++) {
        close(piped[i][1]);
        if (i != N - 1) close(piped[i][0]);
    }

    /*leggiamo ciò che è stato inviato dall'ultimo figlio*/
    nr = read(piped[N-1][0],&A,sizeof(A));
    if( nr != sizeof(A)) {
        printf("Errore: il  ha letto un numero di byte sbagliati\n");
        exit(-1);
    }

    printf("Figlio avente pid %d, ha ritornato al seguente struttura\n",pid[A.c2]);
    printf("c1-> %ld c2->%d\n",A.c1,A.c2);


    /*Il padre aspetta tutti i processi figli*/
    for(j=0;j<N;j++) {
        if((pidFiglio = wait(&status)) < 0) {
            printf("Errore wait\n");
            exit(4);
        }
        if((status & 0xFF) != 0) {
            printf("Figlio con pid %d terminato in modo anomalo\n",pidFiglio);
             exit(5);
        } else {
             ritorno = (int)((status >> 8) & 0xFF);
             printf("Il figlio con pid = %d ha ritornato %d (se 255 problemi!)\n",pidFiglio,ritorno);
         }
    }
    exit(0);
}
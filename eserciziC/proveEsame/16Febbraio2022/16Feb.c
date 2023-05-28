#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>

typedef int pipe_t[2];
typedef struct {
    int c1; /*pid del processo*/
    long int c2; /*numero di occorenze del carattere*/
} structer;

int main(int argc, char **argv) {

    int N;
    char C;
    int fd;
    int j,k;
    int pid,status,ritorno;
    pipe_t *pipes;
    structer *A;
    char ch;
    long int occ;
    int nr,nw;



    /*controllo sul numero di parametri*/
    if (argc < 4) {
        printf("Errore numero di parametri, dato che argc=%d\n",argc);
        exit(1);
    }
    N = argc -2;

    /*controllo sull'ultimo parametro*/
    C = argv[argc-1][0];
    if (strlen(argv[argc-1]) != 1) {
        printf("Errore,ultimo parametro inviato non corretto, deve essere un singolo carattere\n");
        exit(2);
    }

  

    /*allocazione pipe*/
    if ((pipes = (pipe_t *)malloc(N*sizeof(pipe_t))) == NULL) {
        printf("Errore allocazione pipe\n");
        exit(4);
    }

    /*creazione pipe*/
    for (j=0;j<N;j++) {
        if (pipe(pipes[j]) < 0) {
            printf("Errore allocazione pipe\n");
            exit(5);
        }
    }

    /*creazione figli*/
    for(j=0;j<N;j++) {

        if ((pid = fork()) < 0) {
            printf("Errore nella fork\n");
            exit(6);
        }
       

        if (pid == 0) {
            /*chiusura delle pipe non utilizzate*/
            for(k =0;k < N;k++) {
                if (k != j) {
                    close(pipes[k][1]);
                }
                if ((j==0) || (k != j-1)) {
                    close(pipes[k][0]);
                }
            }
        

            /*allochiamo memoria per l'array di struct*/
            if ((A = (structer*)malloc((j+1)*sizeof(structer))) == NULL) {
                printf("Errore allocazine memoria\n");
                exit(3);
            }

            /*apriamo il file*/
            if ((fd = open(argv[j+1],O_RDONLY)) < 0) {
                printf("Errore nella apertura del file\n");                
                exit(-1);            
            }

            /*ogni figlio deve leggere il proprio file*/
            while(read(fd,&ch,sizeof(ch))) {
                if (C==ch) {
                    occ++;
                }
            }

            /*il figlio di indice 0 deve preparare la struttura da mandare al figlio seguente*/
            if (j == 0) {
                A[0].c1 = getpid();
                A[0].c2 = occ;
            } else {
                /*lettura della pipe delle struttura per tutti i figli tranne il primo*/
                nr = read(pipes[j-1][0],A,j*sizeof(structer));
                
                if (nr != j*sizeof(structer)) {
                    printf("Figlio %d ha letto un numero di byte sbagliati\n",j);
                    exit(7);
                }

                A[j].c1 = getpid();
                A[j].c2 = occ;
            }
            /*tutti i figlio mandano in avanti, l'ultimo figlio manda al padre una strtuttura*/
            nw = write(pipes[j][1],A,(j+1)*sizeof(structer));

            if (nw != (j+1)*sizeof(structer)) {
                    printf("Figlio %d ha scritto un numero di byte sbagliati\n",j);
                    exit(8);
            }
            exit(j);
        }
    }
    
    
    
    
    
    
    
    /*codice del padre*/
    
    /*chiudiamo le pipe non utilizzate*/
    for(j=0;j<N;j++) {
        close(pipes[j][1]);
        if (j != N-1) close(pipes[j][0]);
    }
    
    /*allochiamo memoria per l'array di struct*/
    if ((A = (structer*)malloc(N*sizeof(structer))) == NULL) {
        printf("Errore allocazine memoria\n");
        exit(3);
    }
   
    /*il padre deve leggere la singola struttura inviata dall'ultimo figlio*/
    nr = read(pipes[N-1][0],A,N*sizeof(structer));

    if (nr != N*sizeof(structer)) {
           printf("Figlio %d ha letto un numero di byte sbagliati\n",j);
           exit(7);
    }

    for (j=0;j<N;j++) {
        printf("Il figlio %d ha ritornato: pid->%d numero_occorenze->%ld\n",j,A[j].c1,A[j].c2);
    }

    /*il padre aspetta il figlio*/
    for (j=0;j<N;j++) {
        if ((pid= wait(&status)) < 0) {
            printf("Errore nella wait\n");
            exit(5);               
        }

        if ((status & 0xFF) != 0) {
                printf("Figlio con pid %d associato al file %s terminato in modo anomalo\n",pid,argv[j+1]);
        }
        else {
            ritorno = (int)((status >> 8) & 0xFF);
            printf("Figlio con pid %d associato al file %s terminato in modo normale e ha ritornato %d(se 255 errore)\n",pid,argv[j+1],ritorno);  
        } 
    }
    exit(0);

}
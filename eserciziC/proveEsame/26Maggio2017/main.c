#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
typedef int pipe_t[2];

struct {
    long int c1; /*valore massimo di occorrenze calcolato dal processo*/
    int c2; /*deve contenere l'indice del processo*/
    long int c3; /*contiene la somma di tutte le occorrenze calcolate dai processi*/
} typedef structer;

int main(int argc,char **argv) {
    /*--------------*variabili locali*--------------*/
    int N;	/*numero di processi figli*/
    int i,j;
    long int k;
    char Cx; /*ultimo parametro che rappresenta un singolo carattere*/
    char ch;
    int fd;	/*file descriptor per apertura file*/
    int nr,nw;	/*per controllare lettura e scrittura pipe*/
    int *pid;
    int pidFiglio,status,ritorno;	/*per fork e wait*/
    pipe_t *piped;
    structer A;
    /*----------------------------------------------*/

    /*controllo sul numero di parametri*/
    if (argc < 4) {
        printf("Errore numero di parametri, dato che argc=%d\n",argc);
        exit(1);
    }  
    
    N = argc-2;
    
    /*Controllo che  sia un singolo carattere*/
    if (strlen(argv[argc-1]) != 1) {
        printf("Errore: parametro passato %s non è un singolo carettere\n",argv[argc -1]);
        exit(2);
    }  
    Cx = argv[argc-1][0];

    /*allocazione dell'arrya di N pid*/
    pid = (int*)malloc(N*sizeof(int));
    if (pid == NULL) {
        printf("Errore: problemi nell'allocazione di mememoria per l'array di pid\n");
        exit(3);
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

    //printf("DEBUG-Sono il processo padre con pid %d e sto per creare %d figli \n",getpid(),N);
    for(i=0;i<N;i++) {
    
        if((pid[i] = fork()) < 0) {
            printf("Errore nella fork\n");
            exit(5);
        }
        if(pid[i]== 0) {
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

            k = 0L; /*contatore per il numero di occorrenze del carattere Cx*/
            while(read(fd,&ch,1)) {
                if (ch == Cx) k++;
            }

            
            if (i != 0) {
                nr = read(piped[i-1][0],&A,sizeof(A));
                if( nr != sizeof(A)) {
                    printf("Errore: il figlio di indice %d ha letto un numero di byte sbagliati\n", i);
                    exit(6);
                }

                if (k > A.c1) {
                    A.c1 = k;
                    A.c2 = i;
                }
                A.c3 += k;
            } else {
                A.c1 = k;
                A.c2 = 0;
                A.c3 = k;

            }

            nw = write(piped[i][1],&A,sizeof(A));
            if( nw != sizeof(A)) {
                printf("Errore:il figlio ha scritto un numero di byte sbagliati\n");
                exit(7);
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

    /*il padre legge la struttura che gli è stata inviata*/
    nr = read(piped[N-1][0],&A,sizeof(A));
    if( nr != sizeof(A)) {
        printf("Errore: il padre ha letto un numero di byte sbagliati\n");
        exit(-1);
    }

    printf("Il filglio con pid %d ha ritornato la seguente struttura\n",pid[A.c2]);
    printf("c1->%ld, c2->%d, c2->%ld\n",A.c1,A.c2,A.c3);

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
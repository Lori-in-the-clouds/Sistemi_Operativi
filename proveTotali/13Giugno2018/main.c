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
    int c1; /*contine il pid del processo figlio*/
    int c2; /*contine la lunghezza della linea corrente compreso il terminatore di linea*/
    int c3; /*indice aggiunta*/
} structer;

int comp(const void *a,const void *b) {
    structer argc1 = *(structer*)a;
    structer argc2 = *(structer*)b;

    if (argc1.c2 > argc2.c2) return 1;
    if (argc1.c2 < argc2.c2) return -1;
    else return 0;
}


int main(int argc,char **argv) {
    /*--------------*variabili locali*--------------*/
    int N;	/*numero di processi figli*/
    int i,j,k;	/*indici*/
    char linea[255]; /*supponiamo che la lunghezza massima di ogni linea sia uguale a 255, incluso il terminatore di linea*/
    int fd;	/*file descriptor per apertura file*/
    int nr,nw;	/*variabili per salvare/controllare valori di ritorno di read/write da/su pipe*/
    int pid;	/*process identifier per le fork()*/
    int pidFiglio,status,ritorno;	/*per valore di ritorno figli*/
    int Y; /*parametro strettmanete positivo che indica la lunghezza in linee del file*/
    structer *A;
    pipe_t *piped;	/*array dinamico di pipe descriptors per comunicazioni figli-padre*/
    /*----------------------------------------------*/

    /*controllo sul numero di parametri*/
    if (argc < 4) {
        printf("Errore numero di parametri, dato che argc=%d\n",argc);
        exit(1);
    }   

    N=argc-2; /*inzializziamo la variabile N*/
    
    /*Controllo che  sia un numero strettamente positivo*/
    if (atoi(argv[argc-1]) < 0) {
        printf("Errore: parametro passato %s non è un numero strettente positivo\n",argv[argc-1]);
        exit(2);
    }

    Y = atoi(argv[argc-1]); /*inizializziamo la variabile Y*/

    /*allocazione dell'array di N pipe descriptors*/
    piped= (pipe_t*)malloc(N*sizeof(pipe_t));
    if (piped == NULL) {
        printf("Errore: problemi nell'allocazione di memoria per la pipe\n");
        exit(3);
    }

    /*Creazione delle N pipe*/
    for(j=0;j<N;j++) {
        if(pipe(piped[j]) < 0) {
            printf("Errore: problemi nella creazione della pipe\n");
            exit(4);
        }
    }

    //printf("DEBUG-Sono il processo padre con pid %d e sto per creare %d figli \n",getpid(),N);
    
    /*Ciclo di generazione dei figli*/
    for(i=0;i<N;i++) {
    
        if((pid = fork()) < 0) {
            printf("Errore nella fork\n");
            exit(5);
        }

        pidFiglio = getpid();
        
        if(pid == 0) {
            /*codice figlio*/
            
            /*chiusura pipes inutilizzate*/
            for (j=0;j<N;j++) {
                if (j!=i)
                    close(piped[j][1]);
                if ((i==0) || (j != i-1))
                    close(piped[j][0]);
            }
            
            /*alloco l'array*/
            A= (structer*)malloc((i+1)*sizeof(structer));
            /*controllo se la l'allocazione sia andata a buon fine*/
            if (A == NULL) {
                printf("Errore nella allocazione della struct\n");
                exit(-1);
            }

            /*apro il file*/
            if ((fd = open(argv[i+1],O_RDONLY))<0) {
                 printf("Errore: impossibile aprire il file %s\n",argv[i+1]);
                exit(-1);
            }

            k=0; /*indice di linea*/
            while(read(fd,&linea[k],1)) {
                
                if (linea[k]=='\n') {
                    /*siamo arrivati alla terminazione della linea*/

                    k++; /*contiamo anche il terminatore di stringa*/
                    if (i==0) {
                         A[0].c1 = pidFiglio;
                         A[0].c2 = k;
                         A[0].c3 = 0;
                    }
                
                    else {
                        /*leggiamo dalla pipe*/
                        nr = read(piped[i-1][0],A,i*sizeof(structer));
                        if( nr != i*sizeof(structer)) {
                            printf("Errore: lettura di un numero di byte sbagliati %d\n",nr);
                            exit(-1);
                        }
                        /*inseriamo i valori trovati all'interno dell'array di struct nella posizione corretta*/
                        A[i].c1 = pidFiglio;
                        A[i].c2 = k;
                        A[i].c3 = i;


                    }
                    
                    

                    /*scriviamo sulla pipe*/
                    nw = write(piped[i][1],A,(i+1)*sizeof(structer));
                    if( nw != (i+1)*sizeof(structer)) {
                        printf("Errore: scrittura di un numero di byte sbagliati %d\n",nw);
                        exit(-1);
                    }

                    /*azzeriamo l'indice k*/
                    k=0;
                } else k++; /*incrementiamo la variabile k*/
            }

            ritorno = i;
            exit(ritorno);
        }
    }

    /*chiusura pipes inutilizzate*/
    for (j=0;j<N;j++) {
        if (j!=i)
            close(piped[j][1]);
        if ((i==0) || (j != i-1))
            close(piped[j][0]);
    }

    /*alloco l'array*/
    A= (structer*)malloc((N)*sizeof(structer));
        /*controllo se la l'allocazione sia andata a buon fine*/
        if (A == NULL) {
            printf("Errore nella allocazione della struct\n");
            exit(-1);
        }

    for (i=0;i<Y;i++) {
        /*leggiamo dalla pipe*/
        nr = read(piped[N-1][0],A,(N)*sizeof(structer));
       
        if( nr != N*sizeof(structer)) {
            printf("Errore: lettura di un numero di byte sbagliati %d\n",nr);
            exit(6);
        }


        //qsort(A,N,sizeof(structer),comp);
        printf("Struttura ordinata riguardo alla linea %d:\n",i);
        for (j=0;j<N;j++) {
            printf("c1->%d c2->%d per file %s\n",A[j].c1,A[j].c2,argv[A[j].c3+1]);
        }
    }

    /*Il padre aspetta tutti i processi figli*/
    for(j=0;j<N;j++) {
        if((pidFiglio = wait(&status)) < 0) {
            printf("Errore wait\n");
            exit(7);
        }
        if((status & 0xFF) != 0) {
            printf("Figlio con pid %d terminato in modo anomalo\n",pidFiglio);
             exit(8);
        } else {
             ritorno = (int)((status >> 8) & 0xFF);
             printf("Il figlio con pid = %d ha ritornato %d (se 255 problemi!)\n",pidFiglio,ritorno);
         }
    }
    exit(0);




}
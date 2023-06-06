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
    int c1; /*contiene il pid del processo figlio*/
    char c2; /*deve contenere il secondo carettere*/
    char c3; /*deve contenere il penultimo parametro della linea*/
} structer;

int main(int argc,char **argv) {
    /*--------------*variabili locali*--------------*/
    int q,j,k;	/*indici*/
    int fd;	/*file descriptor per apertura file*/
    int pid;	/*process identifier per le fork()*/
    int pidFiglio,status,ritorno;	/*per valore di ritorno figli*/
    int L; /*varaibie nella quale è memorizzzato il secondo parametro*/
    pipe_t *piped;	/*array dinamico di pipe descriptors per comunicazioni figli-padre*/
    int linea; /*indice di linea*/
    structer A;
    char linee[255];
    
    /*----------------------------------------------*/

    /*controllo sul numero di parametri*/
    if (argc < 3) {
        printf("Errore numero di parametri, dato che argc=%d\n",argc);
        exit(1);
    }   

    /*Controllo che  sia un numero strettamente positivo*/
    if (atoi(argv[argc-1]) < 0) {
        printf("Errore: parametro passato %s non è un numero strettente positivo\n",argv[argc-1]);
        exit(2);
    }

    L=atoi(argv[argc-1]);

    /*allocazione dell'array di N pipe descriptors*/
    piped= (pipe_t*)malloc(L*sizeof(pipe_t));
    if (piped == NULL) {
        printf("Errore: problemi nell'allocazione di memoria per la pipe\n");
        exit(2);
    }

    /*Creazione delle N pipe*/
    for(j=0;j<L;j++) {
        if(pipe(piped[j]) < 0) {
            printf("Errore: problemi nella creazione della pipe\n");
            exit(3);
        }
    }

    printf("DEBUG-Sono il processo padre con pid %d e sto per creare %d figli \n",getpid(),L);
    
    /*Ciclo di generazione dei figli*/
    for(q=0;q<L;q++) {
    
        if((pid = fork()) < 0) {
            printf("Errore nella fork\n");
            exit(4);
        }

        pidFiglio = getpid();
        
        if(pid == 0) {
            /*codice figlio*/
            
            /*chiudiamo pipe che non utilizziamo*/
            for (k=0;k<L;k++) {
                close(piped[k][0]);
                if(q != k) close(piped[k][1]);
            }

            /*apro il file*/
            if ((fd = open(argv[1],O_RDONLY))<0) {
                 printf("Errore: impossibile aprire il file %s\n",argv[1]);
                exit(-1);
            }

            linea=1; /*inizializziamo la variabile linea*/
            k=0;
            while(read(fd,&linee[k],1)) {
                
                if (linee[k] == '\n') {
                    
                    if (linea == q+1) {
                        A.c1 = pidFiglio;
                        A.c2 = linee[1];
                        A.c3 = linee[k-1];
                        /*scriviamo sulla pipe verso il padre*/
                        write(piped[q][1],&A,sizeof(A));
                    }
                    linea++;
                    k=0;
                } else {
                    k++;
                }
            }
            exit(q);
            
        }
    }

    /*codice padre*/
    /*chiudiamo le pipe che non utilizziamo*/
    for (j=0;j<L;j++) {
        close(piped[j][1]);
    }

    /*leggiamo i dati proventite dai figlli*/
    for (q=0;q<L;q++) {
        read(piped[q][0],&A,sizeof(A));
        
        if (A.c3 == A.c2) {
            printf("Il figlio di indice %d riguardo alla linea %d ha riportato al seguente struttura: c1->%d c2->%c c3->%c\n",q,q+1,A.c1,A.c2,A.c3);
        }
    }

    /*Il padre aspetta tutti i processi figli*/
    for(j=0;j<L;j++) {
        if((pidFiglio = wait(&status)) < 0) {
            printf("Errore wait\n");
            exit(5);
        }
        if((status & 0xFF) != 0) {
            printf("Figlio con pid %d terminato in modo anomalo\n",pidFiglio);
             exit(6);
        } else {
             ritorno = (int)((status >> 8) & 0xFF);
             printf("Il figlio con pid = %d ha ritornato %d (se 255 problemi!)\n",pidFiglio,ritorno);
         }
    }
    exit(0);

}
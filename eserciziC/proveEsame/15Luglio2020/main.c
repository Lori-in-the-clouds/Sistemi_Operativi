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
    int c1; /*deve contenere il pid del processo figlio*/
    char c2; /*deve contenenre il secondo carattere*/
    char c3; /*deve contenere il penultimo carattere*/
} structer;



int main(int argc,char **argv) {
    /*--------------*variabili locali*--------------*/
    int L;
    int i,j,k;
    char ch;
    int fd;	/*file descriptor per apertura file*/
    int pid;
    int pidFiglio,status,ritorno;	/*per fork e wait*/
    char linea[255];
    pipe_t *piped;
    structer A;
    /*----------------------------------------------*/

    /*controllo sul numero di parametri*/
    if (argc < 3) {
        printf("Errore numero di parametri, dato che argc=%d\n",argc);
        exit(1);
    }
   
    /*Controllo che  sia un numero strettamente positivo*/
    if (atoi(argv[2]) < 0 || atoi(argv[2]) > 255) {
        printf("Errore: parametro passato %s non è un numero strettente positivo\n",argv[2]);
        exit(2);
    }
    
    L = atoi(argv[2]);

    /*allocazione dell'array di N pipe descriptors*/
    piped= (pipe_t*)malloc(L*sizeof(pipe_t));
    if (piped == NULL) {
        printf("Errore: problemi nell'allocazione di memoria per la pipe\n");
        exit(3);
    }

    for(j=0;j<L;j++) {
        if(pipe(piped[j]) < 0) {
            printf("Errore: problemi nella creazione della pipe\n");
            exit(4);
        }
    }

    printf("DEBUG-Sono il processo padre con pid %d e sto per creare %d figli \n",getpid(),L);
    for(i=0;i<L;i++) {
    
        if((pid = fork()) < 0) {
            printf("Errore nella fork\n");
            exit(5);
        }
        if(pid == 0) {
            /*codice figlio*/
            
            /*chiudiamo pipe che non utilizziamo*/
            for (k=0;k<L;k++) {
                close(piped[k][0]);
                if(i != k) close(piped[k][1]);
            }

            /*apro il file*/
            if ((fd = open(argv[1],O_RDONLY))<0) {
                 printf("Errore: impossibile aprire il file %s\n",argv[1]);
                exit(-1);
            }

           k = 0; /*lo utilizziamo come indice per una linea*/
           j = 0;
           while(read(fd,&ch,1)) {
                
                if (ch == '\n') {

                    if (j == i) {
                        /*siamo arrivati alla fine della linea*/
                        A.c1 = getpid();
                        A.c2 = linea[1];
                        A.c3 = linea[k-1];

                        /*scriviamo al padre*/
                        write(piped[i][1],&A,sizeof(A));
                        break;
                    }
                    k = 0;
                    j++;
                }
                else {
                    linea[k] = ch;
                    k++;
                }

           }
           ritorno = j;
           exit(ritorno);
        }
    }

    /*codice figlio*/
    /*chiudiamo le pipe che non utilizziamo*/
    for (j=0;j<L;j++) {
        close(piped[j][1]);
    }

    for (i=0;i<L;i++) {
        /*leggiamo le informazioni inviate dal figlio*/
        read(piped[i][0],&A,sizeof(A));
        if (A.c2 == A.c3) {
        printf("il figlio di indice %d(=linea) e pid %d ha riportato al seguente struttura: c2->%c c3->%c\n",i,A.c1,A.c2,A.c3);
        }
    }

    /*Il padre aspetta tutti i processi figli*/
    for(j=0;j<L;j++) {
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
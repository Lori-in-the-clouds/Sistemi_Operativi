#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>

typedef int pipe_t[2];

int main(int argc, char **argv) {
    
    int N;
    int fd;
    int ritorno,status,pidFiglio;
    int *pid;
    int j,k;
    pipe_t *piped_FP;
    pipe_t *piped_PF;
    char indicazione,ch,ch1;
    int fdout;
    


    /*controllo sul numero di parametri*/
    if (argc < 4) {
        printf("Errore numero di parametri, dato che argc=%d\n",argc);
        exit(1);
    }
    N = argc-2;

    int arr[N];

    for(j=0;j<N;j++) {
        arr[j]=1;
    }
    
    
    pid = (int*)malloc(N*sizeof(int));

    /*allocazione dell'array di N pipe descriptors*/
    if ((piped_FP = (pipe_t*)malloc(N*sizeof(pipe_t))) == NULL || (piped_PF = (pipe_t*)malloc(N*sizeof(pipe_t))) == NULL ) {
        printf("Errore nella creazione della pipe\n");
        exit(4);
    }

    /*creazione delle N pipe figli-padre*/   
    for (j=0;j<N;j++) {
            if (pipe(piped_FP[j]) < 0 || pipe(piped_PF[j]) < 0) {
                printf("Errore nella creazione della pipe\n");
                exit(5);
            }
    }

    /*apriamo il file utilizzato dal padre*/
    if ((fdout = open(argv[argc -1],O_RDONLY)) < 0) {
        printf("Errore  nella apertura del file del padre\n");
        exit(-1);
    }

    /*generazione processi figli*/
    for (j=0;j<N;j++) {

        if ((pid[j] = fork()) < 0) {
            printf("Errore nella fork\n");
            exit(2);
        }

        if (pid[j] == 0) {
            /*chiudiamo le pipe che non utilizziamo*/
            for (k=0;k<N;k++) {
                close(piped_FP[k][0]);
                close(piped_PF[k][1]);

                if (k != j) {
                    close(piped_FP[k][1]);
                    close(piped_PF[k][0]);
                }
            }

            /*apriamo il file*/
            if ((fd = open(argv[j+1],O_RDONLY)) < 0) {
                printf("Errore nella apertura del file\n");                
                exit(-1);
            }

            /*leggiamo l'indicazione del padre*/
            while(read(piped_PF[j][0],&indicazione,sizeof(indicazione))) {

                if (indicazione == 't') break;
                else  {
                    /*il figlio può procedere*/

                    /*leggiamo dal file un carattere*/
                    read(fd,&ch,sizeof(ch));

                    /*inviamo tale carattere al padre*/
                    write(piped_FP[j][1],&ch,1);
                }
                
            }
            exit(0);
        }
    }
    
    /*codice padre*/

    /*chiudiamo i lati delle pipe che non utilizziamo*/
    for (j=0;j<N;j++) {
        close(piped_FP[j][1]);
        close(piped_PF[j][0]);
    }

    /*leggiamo il file del padre*/
    while(read(fdout,&ch,sizeof(ch))) {
         
         for (j=0;j<N;j++) {
            /*il padre confronta tale carattere con tutti i caratteri inviati dai figli*/
            if (arr[j] == 1) {
                indicazione = 'v';
                write(piped_PF[j][1],&indicazione,1);

                /*leggiamo cio che arriva dal figlio*/
                read(piped_FP[j][0],&ch1,1);

                if (ch != ch1) {
                    arr[j] = 0;
                }
            }
         }
    }

         /*terminata la lettura del file AF*/
         for (j=0;j<N;j++) {
            if (arr[j] == 0) {
                
                /*terminiamo forzatamente i figli selezinati*/
                printf("Uccido il processo con pid %d\n",pid[j]);
                if (kill(pid[j],SIGKILL) == -1) {
                    printf("Il figlio con pid %d non esiste e quindi è già terminato\n",pid[j]);
                }
            } else {

                /*per chi non è fallito mandiamo l'indicazine di terminare*/
                indicazione = 't';
                write(piped_PF[j][1],&indicazione,1);
            }
         }

         /*il padre aspetta il figlio*/
        for (j=0;j<N;j++) {
                if ((pidFiglio = wait(&status)) < 0) {
                    printf("Errore nella wait\n");
                    exit(5);
                }

                if ((status & 0xFF) != 0) {
                    printf("Figlio con pid %d associato al file %s terminato in modo anomalo\n",pidFiglio,argv[j+1]);
                }
                else {
                    ritorno = (int)((status >> 8) & 0xFF);
                    printf("Figlio con pid %d associato al file %s terminato in modo normale e ha ritornato %d(se 255 errore)\n",pidFiglio,argv[j+1],ritorno);  
                } 
        }
        exit(0);
    
}
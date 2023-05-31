#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
typedef int pipe_t[2];

int main(int argc,char **argv) {
    /*--------------*variabili locali*--------------*/
    int N;	/*numero di processi figli*/
    int i,j,k,n;
    int fd;	/*file descriptor per apertura file*/
    int nr,nw;	/*per controllare lettura e scrittura pipe*/
    int pid;
    int pidFiglio,status,ritorno;	/*per fork e wait*/
    int nroLinee;
    int fdout; /*file descriptor per il file creato*/
    pipe_t *piped;
    char linea[255];
    /*----------------------------------------------*/

    /*controllo sul numero di parametri*/
    if (argc < 4) {
        printf("Errore numero di parametri, dato che argc=%d\n",argc);
        exit(1);
    }   
    N = argc-2;

    /*Controllo che  sia un numero strettamente positivo*/
    if (atoi(argv[argc-1]) < 0) {
        printf("Errore: parametro passato %s non è un numero strettente positivo\n",argv[argc-1]);
        exit(2);
    }
    nroLinee = atoi(argv[argc-1]);

    /*creiamo un file con il nostro cognome*/
    if ((fdout = creat("DIMAIO",0644)) < 0) {
        printf("Errore nella creazione del file\n");
        exit(3);
    }

    /*allocazione dell'array di N pipe descriptors*/
    piped= (pipe_t*)malloc(N*sizeof(pipe_t));
    if (piped == NULL) {
        printf("Errore: problemi nell'allocazione di memoria per la pipe\n");
        exit(4);
    }

    for(j=0;j<N;j++) {
        if(pipe(piped[j]) < 0) {
            printf("Errore: problemi nella creazione della pipe\n");
            exit(5);
        }
    }

    char tutteLinee[N][255];

   

    printf("DEBUG-Sono il processo padre con pid %d e sto per creare %d figli \n",getpid(),N);
    for(n=0;n<N;n++) {
    
        if((pid = fork()) < 0) {
            printf("Errore nella fork\n");
            exit(-1);
        }
        if(pid == 0) {
            /*codice figlio*/
            
            /*chiusura pipes inutilizzate*/
            for (j=0;j<N;j++) {
                if (j!=n)
                    close(piped[j][1]);
                if ((n==0) || (j != n-1))
                    close(piped[j][0]);
            }

            /*apro il file*/
            if ((fd = open(argv[n+1],O_RDONLY))<0) {
                 printf("Errore: impossibile aprire il file %s\n",argv[n+1]);
                exit(-1);
            }

            k=0; /*indice*/
            while(read(fd,&linea[k],1)) {

                if (linea[k] == '\n') {
                    if (n != 0) {
                        
                        /*leggiamo dalla pipe precendente*/
                        nr = read(piped[n-1][0],tutteLinee,sizeof(tutteLinee));
                        if( nr != sizeof(tutteLinee)) {
                            printf("Errore: il  ha letto un numero di byte sbagliati\n");
                            exit(-1);
                        }
                    }
                    k++;
                    linea[k]='\0'; //ATTENZIONE
                    /*aggiungiamo tale linea all'array di linea*/
                    for (j=0;j<k;j++) {
                        tutteLinee[n][j]= linea[j];
                    }
                    /*scriviamo alla pipe successiva*/

                    nw = write(piped[n][1],tutteLinee,sizeof(tutteLinee));
                    if( nw != sizeof(tutteLinee)) {
                        printf("Errore:il ha scritto un numero di byte sbagliati\n");
                        exit(-1);
                    }

                    k = 0;
                } else {
                    k++;
                }
            }

            exit(strlen(linea));
        }
    }

    /*codice padre*/
    /*chiusura pipe: tutte meno l'ultima in lettura*/
    for (i=0;i<N;i++) {
        close(piped[i][1]);
        if (i != N - 1) close(piped[i][0]);
    }

    for (j=1;j<=nroLinee;j++) {
        nr = read(piped[n-1][0],tutteLinee,sizeof(tutteLinee));
        if( nr != sizeof(tutteLinee)) {
            printf("Errore: il  ha letto un numero di byte sbagliati\n");
            exit(8);
        }

        for (i=0;i<N;i++) {

            for (k=0;k<250;k++) {
                write(fdout,&(tutteLinee[i][k]),1);
                if (tutteLinee[i][k] == '\n') {
                    break;
                }
            }
        }
        
    }
    /*Il padre aspetta tutti i processi figli*/
    for(n=0;n<N;n++) {
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
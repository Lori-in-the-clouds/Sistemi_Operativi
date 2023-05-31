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
    int i,n,j,k;	/*indici*/
    int fd;	/*file descriptor per apertura file*/
    int nr,nw;	/*variabili per salvare/controllare valori di ritorno di read/write da/su pipe*/
    int pid;	/*pid per fork*/
    int pidFiglio,status,ritorno;	/*per valore di ritorno figli*/
    int fdout;
    pipe_t *piped;	/*array di pipe usate a ring da primo figlio, a secondo figlio .... ultimo figlio e poi a primo figlio: ogni processo legge dalla pipe q e scrive sulla pipe (q+1)%Q*/
    /*----------------------------------------------*/

    /*controllo sul numero di parametri*/
    if (argc < 3) {
        printf("Errore numero di parametri, dato che argc=%d\n",argc);
        exit(1);
    }

    N = argc -1;
    char linee[N][250];

    /*creiamo il file*/
    if ((fdout = creat("DIMAIO",0644)) < 0) {
        printf("Errore nella creazione del file\n");
        exit(1);
    }
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
    for(n=0;n<N;n++) {
    
        if((pid = fork()) < 0) {
            printf("Errore nella fork\n");
            exit(4);
        }
        if(pid == 0) {
            /*codice figlio*/
            /*chiudiamo pipe che non utilizziamo*/
            /*ATTENZIONE DA CANCELLARE: q oppure i*/
            for(j=0;j<N;j++) {
                if(j != n)
                    close(piped[j][0]);
                if(j != (n+1)%N)
                    close(piped[j][1]);
            }

            /*apro il file*/
            if ((fd = open(argv[n+1],O_RDONLY))<0) {
                 printf("Errore: impossibile aprire il file %s\n",argv[n+1]);
                exit(-1);
            }

            char linea[250];
            k = 0; /*indice per riga*/
            while(read(fd,&linea[k],1)) {
                
                if (linea[k] == '\n') {
                    k++;
                    /*inseriamo il terminatore di linea*/
                    linea[k]='\0';

                    /*legggiamo l'array di stringhe proveniente dal figlio precedente*/
                    nr = read(piped[n][0],linee,sizeof(linee));
                    if( nr != sizeof(linee)) {
                        printf("Errore: lettura di un numero di byte sbagliati %d\n",nr);
                        exit(5);
                    }

                    /*inseriamo la nuova linea dentro l'array*/
                    for (j=0;j<k;j++) {
                        linee[n][j] = linea[j];
                    }

                    /*se è l'ultimo figlio stampiamo*/
                    if (n == N -1) {
                        for(i=0;i<N;i++) {
                            for (j=0;j<250;j++) {
                                write(fdout,&linee[i][j],1);
                                if (linee[i][j]=='\n') break;
                            }
                        }
                    }

                    /*invaimo l'array di stringhe*/
                    nw = write(piped[(n+1)%N][1],&linee,sizeof(linee));
                    if( nw != sizeof(linee)) {
                        printf("Errore: scrittura di un numero di byte sbagliati %d\n",nw);
                        exit(6);
                    }
                    ritorno = strlen(linea);
                    k = 0;

                } else {
                    k++;
                }
            }
            exit(ritorno);

        }
    }

    /*codice padre*/
    /*chiudiamo le pipe che non utilizziamo, apparte la prima perchè il padre deve dare il primo ok al primo figlio*/
    /*si lascia aperto sia il lato di scrittura che viene usata (e poi in effetti chiuso) che il lato di lettura*/
    for (j=1;j<N;j++) {
        close(piped[j][1]);
        close(piped[j][0]);
    }
    
    /*ora si deve madare l'ok al primo figlio (P0): il valore della variabile ok non ha importanza*/
    nw= write(piped[0][1],linee,sizeof(linee));
    /*anche in questo caso controlliamo il risultato della scrittura*/
    if(nw != sizeof(linee)) {
        printf("Padre ha scritto un numero di byte sbagliati %d\n",nw);
        exit(-1);
    }
    
    /*ora possiamo chiudere anche il lato di scrittura, ma non quello di lettura*/
    close(piped[0][1]);

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
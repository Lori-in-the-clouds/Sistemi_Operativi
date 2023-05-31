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
    int Q;	/*numero di processi figli*/
    int q,j,k;
    char ch;
    int fd;	/*file descriptor per apertura file*/
    int nr,nw;	/*per controllare lettura e scrittura pipe*/
    int pid;
    int pidFiglio,status,ritorno;	/*per fork e wait*/
    pipe_t *piped;
    char ok; /*carattere letto dai figli dalla pipe precedente e scritta su quella successiva*/
    /*----------------------------------------------*/   

    /*controllo sul numero di parametri*/
    if (argc < 3) {
        printf("Errore numero di parametri, dato che argc=%d\n",argc);
        exit(1);
    }
    Q = argc -1;

    /*allocazione dell'array di N pipe descriptors*/
    piped= (pipe_t*)malloc(Q*sizeof(pipe_t));
    if (piped == NULL) {
        printf("Errore: problemi nell'allocazione di memoria per la pipe\n");
        exit(2);
    }

    for(j=0;j<Q;j++) {
        if(pipe(piped[j]) < 0) {
            printf("Errore: problemi nella creazione della pipe\n");
            exit(3);
        }
    }

    printf("DEBUG-Sono il processo padre con pid %d e sto per creare %d figli \n",getpid(),Q);
    for(q=0;q<Q;q++) {
    
        if((pid = fork()) < 0) {
            printf("Errore nella fork\n");
            exit(-1);
        }
        if(pid == 0) {
            /*codice figlio*/
            /*chiudiamo pipe che non utilizziamo*/
            for(j=0;j<Q;j++) {
                if(j != q)
                    close(piped[j][0]);
                if(j != (q+1)%Q)
                    close(piped[j][1]);
            }
            
            /*apro il file*/
            if ((fd = open(argv[q+1],O_RDONLY))<0) {
                 printf("Errore: impossibile aprire il file %s\n",argv[q+1]);
                exit(-1);
            }

            k = 0; /*indice per la ricerca del carattere*/
            j = 0;
            while(read(fd,&ch,1)) {
                
                if (j == q + (k*Q)) {
                    /*il figlio aspetta l'ok da parte del padre/figlio precedente*/
                    nr = read(piped[q][0],&ok,1);
                    if( nr != 1) {
                        printf("Errore: il  ha letto un numero di byte sbagliati\n");
                        exit(-1);
                    }
                    /*stampo il carattere*/
                    printf("Il figlio di indice %d ha ritornato il carattere %c\n",q,ch);
                    /*inviamo al figlio successivo l'ok*/
                    nw = write(piped[(q+1)% Q][1],&ok,1);
                    if( nw != 1) {
                        printf("Errore:il   ha scritto un numero di byte sbagliati\n");
                        exit(-1);
                    }
                    k++;
                }

                j++;
            }

            exit(q);
            
        }
    }

    /*codice padre*/
    /*chiudiamo le pipe che non utilizziamo, apparte la prima perchè il padre deve dare il primo ok al primo figlio*/
    /*si lascia aperto sia il lato di scrittura che viene usata (e poi in effetti chiuso) che il lato di lettura*/
    for (j=1;j<Q;j++) {
        close(piped[j][1]);
        close(piped[j][0]);
    }

    /*ora mandiamo l'ok al primo figlio*/
    nw = write(piped[0][1],&ok,sizeof(ok));
    if( nw != sizeof(ok)) {
        printf("Errore:il   ha scritto un numero di byte sbagliati\n");
        exit(5);
    }

    /*ora possiamo chiudere anche il lato di scirttura ma non quello di lettura*/
    close(piped[0][1]);


    /*Il padre aspetta tutti i processi figli*/
    for(j=0;j<Q;j++) {
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

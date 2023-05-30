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
    int j,n,k;
    char car;
    int fd;	/*file descriptor per apertura file*/
    int nr,nw;	/*per controllare lettura e scrittura pipe*/
    int pid;
    int pidFiglio,status,ritorno;	/*per fork e wait*/
    int fcreato; /*file descriptor del file creato*/
    pipe_t *piped;
    /*----------------------------------------------*/

    /*controllo sul numero di parametri*/
    if (argc < 3) {
        printf("Errore numero di parametri, dato che argc=%d\n",argc);
        exit(1);
    }
    N = argc-1;

    /*creiamo un file nella directory*/
    if ((fcreato = creat("DIMAIO",0644)) < 0) {
        printf("Errore nella creazione del file\n");
        exit(2);
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

    char cur[N];

    printf("DEBUG-Sono il processo padre con pid %d e sto per creare %d figli \n",getpid(),N);
    for(n=0;n<N;n++) {
    
        if((pid = fork()) < 0) {
            printf("Errore nella fork\n");
            exit(-1);
        }
        if(pid == 0) {
            /*codice figlio*/
            
            /*chiudiamo le pipe che non utilizziamo*/
            for (j=0;j<N;j++) {
                if (j != n) {
                    close(piped[j][1]);
                }
                if ((n==0) || (j != n-1)) {
                    close(piped[j][0]);
                }
            }


            /*apro il file*/
            if ((fd = open(argv[n+1],O_RDONLY))<0) {
                 printf("Errore: impossibile aprire il file %s\n",argv[n+1]);
                exit(-1);
            }

            k = 0; /*lo utilizzo come indice*/

            while(read(fd,&car,1)) {

                if (k % 2 == 1) {

                    if (n != 0) {
                        /*leggiamo ciò che abbiamo ricevuto dal figlio precedente*/
                        nr = read(piped[n-1][0],cur,sizeof(cur));
                        if( nr != sizeof(cur)) {
                            printf("Errore: lettura un numero di byte sbagliati\n");
                            exit(-1);
                        }
                    }
                    
                    /*carattere dispari*/
                    cur[n]=car;

                    /*scriviamo con la pipe al figlio successivo*/
                    nw = write(piped[n][1],cur,sizeof(cur));
                    if( nw != sizeof(cur)) {
                        printf("Errore:il   ha scritto un numero di byte sbagliati\n");
                        exit(-1);
                    }

                }

                k++;
            }
            exit(cur[n]);
            
        }
    }


    /*codice padre*/
    /*chiudo pipe che non utlizzo,tutte meno l'ultima in lettura*/

    for (j=0;j<N;j++) {
        close(piped[j][1]);
        if (j != N-1) close(piped[j][0]);
    }

    /*il padre legge ciò che è stato inviato dal figlio*/
    while(read(piped[N-1][0],cur,sizeof(cur))) {
        write(fcreato,cur,sizeof(cur));
    }

    /*Il padre aspetta tutti i processi figli*/
    for(j=0;j<N;j++) {
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

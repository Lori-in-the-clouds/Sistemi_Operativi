#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
typedef int pipe_t[2];

/*VARIABILI GLOBALI*/
int *finito;	/*array dinamico per indicare i figli che sono terminati*/
            /*la semantica di questo array è che ogni elemento vale 0 se il corrispondente processo NON è finito, altrimenti vale 1*/
int N;		/*numero di processi figli*/

int finitof() {
/*questa funzione verifica i valori memorizzati nell'array finito: appena trova un elemento uguale a 0 vuole dire che non tutti i processi figli sono finiti e quindi torna 0;*/
/*tornerà 1 se e solo se tutti gli elementi dell'array sono a 1 e quindi tutti i processi sono finiti*/
int i;
    for(i=0; i < N; i++)
        if (!finito[i])
            /*appena ne trova uno che non ha finito*/
            return 0;
    return 1;
} //RICORDA DI ALLOCARE FINITO-------

int main(int argc,char **argv) {
    /*--------------*variabili locali*--------------*/
    int i,j,k;
    char ch;
    char inviato[2];
    int fd;	/*file descriptor per apertura file*/
    int nr,nw;	/*per controllare lettura e scrittura pipe*/
    int pid;
    int pidFiglio,status,ritorno;	/*per fork e wait*/
    int fdout;
    pipe_t *piped;
    /*----------------------------------------------*/

    /*controllo sul numero di parametri*/
    if (argc < 4) {
        printf("Errore numero di parametri, dato che argc=%d\n",argc);
        exit(1);
    }
    N = argc -2;

    /*apro il file*/
    if ((fdout = open(argv[N+1],O_WRONLY))<0) {
         printf("Errore: impossibile aprire il file %s\n",argv[N+1]);
        exit(-1);
    }

    //posizionamolo in ultima posizione 
    lseek(fdout,0L,SEEK_END);

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

  
    /*allocazione memoria dinamica per finito*/
    finito = (int*)malloc(N*sizeof(int));
    if (finito == NULL) {
        printf("Errore nella allocazione della memoria per array finito\n");
        exit(4);
    }
    
    /*inizializzazione a 0 di finito: all'inizio nessun figlio è finito;*/
    memset(finito,0,N*sizeof(int));

    printf("DEBUG-Sono il processo padre con pid %d e sto per creare %d figli \n",getpid(),N);
    for(i=0;i<N;i++) {
    
        if((pid = fork()) < 0) {
            printf("Errore nella fork\n");
            exit(-1);
        }
        if(pid == 0) {
            /*codice figlio*/
            /*chiudiamo pipe che non utilizziamo*/
            for (k=0;k<N;k++) {
                close(piped[k][0]);
                if(i != k) close(piped[k][1]);
            }

            /*apro il file*/
            if ((fd = open(argv[i+1],O_RDONLY))<0) {
                 printf("Errore: impossibile aprire il file %s\n",argv[i+1]);
                exit(-1);
            }

            int caratteri_inviati = 0;
            
            while((nr = read(fd,inviato,2)) != 0) {
                /*mandiamo al padre*/
                nw = write(piped[i][1],inviato,nr);
                if (nw != nr) {
                    printf("Errore sulla scrittura in pipe\n");
                    exit(-1);
                }
                caratteri_inviati+=nr;
            }

            exit(caratteri_inviati);
        }
    }

    /*codice padre*/
    /*chiudiamo le pipe che non utilizziamo*/
    for (j=0;j<N;j++) {
        close(piped[j][1]);
    }

    while(!finitof()) {
        for (int i =0;i<N;i++) {

            nr = read(piped[i][0],inviato,2);
            if (nr == 0) {
                finito[i]=1;
            }

            if (!finito[i]) {
               
                /*scriviamo sul file*/
                write(fdout,inviato,nr);
                
            }
        }
    }

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
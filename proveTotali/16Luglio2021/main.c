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
    int n,j,k;	/*indici*/
    int fd;	/*file descriptor per apertura file*/
    int nr,nw;	/*variabili per salvare/controllare valori di ritorno di read/write da/su pipe*/
    int pid;	/*process identifier per le fork()*/
    int pidFiglio,status,ritorno;	/*per valore di ritorno figli*/
    int fcreato; /*file descriptor del file creato secondo consegna*/
    pipe_t *piped;
    char linea[255]; /*parametro che utilizziamo per salvare temporaneamente la linea considerata*/
    /*----------------------------------------------*/

    /*controllo sul numero di parametri*/
    if (argc < 3) {
        printf("Errore numero di parametri, dato che argc=%d\n",argc);
        exit(1);
    }   
    N= argc-1;

    char tutteLinee[N][255];

    
    /*creo un nuovo file*/
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

    /*Creazione delle N pipe*/
    for(j=0;j<N;j++) {
        if(pipe(piped[j]) < 0) {
            printf("Errore: problemi nella creazione della pipe\n");
            exit(4);
        }
    }

    printf("DEBUG-Sono il processo padre con pid %d e sto per creare %d figli \n",getpid(),N);
    
    /*Ciclo di generazione dei figli*/
    for(n=0;n<N;n++) {
    
        if((pid = fork()) < 0) {
            printf("Errore nella fork\n");
            exit(5);
        }
        if(pid == 0) {
            /*codice figlio*/
            
            /*chiudiamo pipe che non utilizziamo*/
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

            k=0; /*indice di linea*/
            while(read(fd,&linea[k],1)) {
                
                if (linea[k]=='\n') {
                    k++; /*consideriamo anche il terminatore di linea*/
                    ritorno = k;
                    /*leggiamo dalla pipe*/
                    nr = read(piped[n][0],tutteLinee,sizeof(tutteLinee));
                    if( nr != sizeof(tutteLinee)) {
                        printf("Errore: lettura di un numero di byte sbagliati %d\n",nr);
                        exit(-1);
                    }

                    /*inseriamo la nuova linea all'interno dell'array*/
                    for (j=0;j<k;j++) {
                        tutteLinee[n][j]=linea[j];
                    }

                    if (n == N-1) {
                        /*se siamo arrivati all'ultimo figlio,scriviamo tale array su fcreato*/
                        for (j=0;j<N;j++) {
                            for (k=0;k<255;k++) {
                                write(fcreato,&tutteLinee[j][k],1);
                                if (tutteLinee[j][k] == '\n') break; /*se siamo arrivati alla linea a capo, passiamo alla linea successiva facendo un break che ci fa uscire dal ciclo più interno*/
                            }
                        }
                    }

                    /*scriviamo sulla pipe*/
                    nw = write(piped[(n+1)%N][1],tutteLinee,sizeof(tutteLinee));
                    if( nw != sizeof(tutteLinee)) {
                        printf("Errore: scrittura di un numero di byte sbagliati %d\n",nw);
                        exit(-1);
                    }
                    k=0;

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
    
    /*inviamo al primo figlio l'array di linee anche se non manda informazioni significative*/
    nw= write(piped[0][1],tutteLinee,sizeof(tutteLinee));
    /*anche in questo caso controlliamo il risultato della scrittura*/
    if(nw != sizeof(tutteLinee)) {
        printf("Padre ha scritto un numero di byte sbagliati %d\n",nw);
        exit(-1);
    }
    
    /*ora possiamo chiudere anche il lato di scrittura, ma non quello di lettura*/
    close(piped[0][1]);

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
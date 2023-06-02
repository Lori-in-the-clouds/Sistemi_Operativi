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
    int j,q;	/*indici*/
    char ch;
    int fd;	/*file descriptor per apertura file*/
    int nr,nw;	/*variabili per salvare/controllare valori di ritorno di read/write da/su pipe*/
    int pid;	/*pid per fork*/
    int pidFiglio,status,ritorno;	/*per valore di ritorno figli*/
    char ok;	/*carattere letto dai figli dalla pipe precedente e scritta su quella successiva*/
    pipe_t *piped;	/*array di pipe usate a ring da primo figlio, a secondo figlio .... ultimo figlio e poi a primo figlio: ogni processo legge dalla pipe q e scrive sulla pipe (q+1)%Q*/
    int occ; /*parametro per contare le occorrenze di un carattere*/
    int linea;
    /*----------------------------------------------*/
    
    /*controllo sul numero di parametri*/
    if (argc < 5) {
        printf("Errore numero di parametri, dato che argc=%d\n",argc);
        exit(1);
    }

    Q = argc -3;

    /*Controllo che il secondo parametro sia un numero strettamente positivo*/
    if (atoi(argv[2]) < 0) {
        printf("Errore: parametro passato %s non è un numero strettente positivo\n",argv[2]);
        exit(2);
    }

    /*Controllo che i restanti parametri siano  singoli caratteri*/
    for (q=0;q<Q;q++) {
        if (strlen(argv[3+q]) != 1) {
            printf("Errore: parametro passato %s non è un singolo carettere\n",argv[3+q]);
            exit(3);
        }
    }

    /*allocazione dell'array di N pipe descriptors*/
    piped= (pipe_t*)malloc(Q*sizeof(pipe_t));
    if (piped == NULL) {
        printf("Errore: problemi nell'allocazione di memoria per la pipe\n");
        exit(4);
    }

    for(j=0;j<Q;j++) {
        if(pipe(piped[j]) < 0) {
            printf("Errore: problemi nella creazione della pipe\n");
            exit(5);
        }
    }
    linea = 1;

    printf("DEBUG-Sono il processo padre con pid %d e sto per creare %d figli \n",getpid(),Q);
    for(q=0;q<Q;q++) {

        if((pid = fork()) < 0) {
            printf("Errore nella fork\n");
            exit(6);
        }
        
        pidFiglio = pid;
        
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
            if ((fd = open(argv[1],O_RDONLY))<0) {
                 printf("Errore: impossibile aprire il file %s\n",argv[1]);
                exit(-1);
            }

            
            /*contiamo le occorenze */
            while(read(fd,&ch,1)) {
                 
                if (ch == '\n') {
                    
                    /*leggiamo l'ok dalla pipe precedente*/
                    nr = read(piped[q][0],&ok,sizeof(ok));
                    if( nr != sizeof(ok)) {
                        printf("Errore: lettura di un numero di byte sbagliati %d\n",nr);
                        exit(-1);
                    }
                    if (q == 0) {
                        printf("Linea %d del file %s\n",linea,argv[1]);
                        linea++;
                    }

                    printf("Figlio con indice %d e pid %d ha letto %d caratteri %c nella linea corrente\n",q,pidFiglio,occ,argv[q+3][0]);

                    /*scriviamo l'ok alla pipe successiva*/
                    nw = write(piped[(q+1)%Q][1],&ok,sizeof(ok));
                    if( nw != sizeof(ok)) {
                        printf("Errore: scrittura di un numero di byte sbagliati %d\n",nw);
                        exit(-1);
                    }
                    ritorno = occ;
                    occ=0;
                }
                if (ch == argv[q+3][0]) occ++;
            }
            exit(ritorno);
        }
    }

    /*codice padre*/
    
    /*chiudiamo le pipe che non utilizziamo, apparte la prima perchè il padre deve dare il primo ok al primo figlio*/
    /*si lascia aperto sia il lato di scrittura che viene usata (e poi in effetti chiuso) che il lato di lettura*/
    for (j=1;j<Q;j++) {
        close(piped[j][1]);
        close(piped[j][0]);
    }
    
    /*ora si deve madare l'ok al primo figlio (P0): il valore della variabile ok non ha importanza*/
    nw= write(piped[0][1],&ok,sizeof(char));
    /*anche in questo caso controlliamo il risultato della scrittura*/
    if(nw != sizeof(char)) {
        printf("Padre ha scritto un numero di byte sbagliati %d\n",nw);
        exit(-1);
    }
    
    /*ora possiamo chiudere anche il lato di scrittura, ma non quello di lettura*/
    close(piped[0][1]);

    
    /*Il padre aspetta tutti i processi figli*/
    for(j=0;j<Q;j++) {
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
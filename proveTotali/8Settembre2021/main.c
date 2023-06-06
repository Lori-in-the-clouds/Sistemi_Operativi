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
}

int main(int argc,char **argv) {
    /*--------------*variabili locali*--------------*/
    int n,j,k;	/*indici*/
    char ch;
    int fd;	/*file descriptor per apertura file*/
    int pid;	/*process identifier per le fork()*/
    int pidFiglio,status,ritorno;	/*per valore di ritorno figli*/
    int fcreato; /*file descriptor del nuovo file creato secondo consegna*/
    pipe_t *pari;
    pipe_t *dispari;
    int linea; /*indice di linea corrente*/
    int nro; /*valore massimo della lunghezza delle linee pari/dispari dei file*/
    int occ; /*indice per contare il numero di occorrenze*/
    /*----------------------------------------------*/

    /*controllo sul numero di parametri*/
    if (argc < 2) {
        printf("Errore numero di parametri, dato che argc=%d\n",argc);
        exit(1);
    } 

    N = argc-1; /*inizializziamo al variabile N*/  

    /*allocazione memoria dinamica per finito*/
    finito = (int*)malloc(N*sizeof(int));
    if (finito == NULL) {
        printf("Errore nella allocazione della memoria per array finito\n");
        exit(2);
    }
    
    /*inizializzazione a 0 di finito: all'inizio nessun figlio è finito;*/
    memset(finito,0,N*sizeof(int));

    /*creo un nuovo file*/
    if ((fcreato = creat("DIMAIO.log",0644)) < 0) {
        printf("Errore nella creazione del file\n");
        exit(2);
    }


   /*allocazione dell'array di N pipe descriptors*/
   pari= (pipe_t*)malloc(N*sizeof(pipe_t));
   dispari= (pipe_t*)malloc(N*sizeof(pipe_t));
   if (pari == NULL || dispari == NULL) {
    printf("Errore: problemi nell'allocazione di memoria per le pipe\n");
    exit(3);
   }

   /*Creazione delle pipe*/
   for(j=0;j<N;j++) {
    if(pipe(pari[j]) < 0 || pipe(dispari[j]) < 0) {
        printf("Errore: problemi nella creazione delle pipe\n");
        exit(4);
    }
   }

   

    printf("DEBUG-Sono il processo padre con pid %d e sto per creare %d figli \n",getpid(),N*2);
    
    /*Ciclo di generazione dei figli*/
    for(n=0;n<N*2;n++) {
    
        if((pid = fork()) < 0) {
            printf("Errore nella fork\n");
            exit(5);
        }
        if(pid == 0) {
            /*codice figlio*/
            if (n % 2 == 1) {
                /*processo dispari*/

                /*chiudiamo pipe che non utilizziamo*/
                for (k=0;k<N;k++) {
                    close(pari[k][0]);
                    close(pari[k][1]);
                    close(dispari[k][0]);

                    if(n/2 != k) {
                        close(dispari[k][1]);
                    }
                }

                /*apro il file*/
                if ((fd = open(argv[(n+1)/2],O_RDONLY))<0) {
                     printf("Errore: impossibile aprire il file %s\n",argv[(n+1)/2]);
                    exit(-1);
                }

                linea=1;
                occ=0;
                while(read(fd,&ch,1)) {

                    /*siamo nella linea dispari*/
                    if (linea % 2 == 1) {
                        occ++;
                    }
                    
                    /*siamo arrivati a capo*/
                    if (ch == '\n') {
                        /*se la linea è dispari*/
                        if (linea % 2 == 1) {

                            /*settiamo il max*/
                            if (occ>nro) nro=occ;
                            
                            /*scriviamo al padre*/
                            write(dispari[n/2][1],&occ,sizeof(occ));
                        }
                        linea++; /*se arrivaimo a capo, incrementiamo l'indice della linea corrente*/
                        occ = 0;
                    }
                }
                exit(nro);

            } else {
                /*processo pari*/

                /*chiudiamo pipe che non utilizziamo*/
                for (k=0;k<N;k++) {
                    close(dispari[k][0]);
                    close(dispari[k][1]);
                    close(pari[k][0]);

                    if(n/2 != k) {
                        close(pari[k][1]);
                    }
                }

                /*apro il file*/
                if ((fd = open(argv[n/2 +1],O_RDONLY))<0) {
                     printf("Errore: impossibile aprire il file %s\n",argv[n/2 +1]);
                    exit(-1);
                }

                linea=1;
                occ=0;
                while(read(fd,&ch,1)) {

                    /*siamo nella linea pari*/
                    if (linea % 2 == 0) {
                        occ++;
                    }
                    
                    /*siamo arrivati a capo*/
                    if (ch == '\n') {
                        /*se la linea è pari*/
                        if (linea % 2 == 0) {

                            /*settiamo il max*/
                            if (occ>nro) nro=occ;
                            
                            /*scriviamo al padre*/
                            write(pari[n/2][1],&occ,sizeof(occ));
                        }
                        linea++; /*se arrivaimo a capo, incrementiamo l'indice della linea corrente*/
                        occ = 0;
                    }
                }
                exit(nro);
            }
        }
    }
    
    /*codice padre*/
    /*chiudiamo le pipe che non utilizziamo*/
    for (j=0;j<N;j++) {
        close(pari[j][1]);
        close(dispari[j][1]);
    }

    
    int rpari;
    int rdispari;
    while(!finitof()) {
        for (n=0;n<N;n++) {
            
            printf("Lunghezza delle linee del file %s sono:\n",argv[n+1]);

            if (!finito[n]) {
                linea=1;
                while(1) {
                    
                    rdispari =read(dispari[n][0],&occ,sizeof(occ));
                    if (rdispari != 0) {
                        printf("Linea numero %d è lunga %d\n",linea,occ);
                        linea++;
                    }
                    
                    rpari = read(pari[n][0],&occ,sizeof(occ));
                    if (rpari != 0) {
                        printf("Linea numero %d è lunga %d\n",linea,occ);
                        linea++;
                    }

                    if ((rdispari + rpari) == 0) {
                        finito[n]=1;
                        break;
                    }
                }
            }

        }
    }
    



    /*Il padre aspetta tutti i processi figli*/
    for(j=0;j<N*2;j++) {
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
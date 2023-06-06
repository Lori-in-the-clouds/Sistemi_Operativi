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
    int i,j,k;	/*indici*/
    char ch;
    int nw,nr;
    int fd;	/*file descriptor per apertura file*/
    int pid;	/*process identifier per le fork()*/
    int pidFiglio,status,ritorno;	/*per valore di ritorno figli*/
    pipe_t *piped_PF;
    pipe_t *piped_FP;
    char linea[255];
    char risposta;
    int stampa; /*variabiel che utlizzo per valutare quante volte il figlio ha stampato*/
    /*----------------------------------------------*/

    /*controllo sul numero di parametri*/
    if (argc < 3) {
        printf("Errore numero di parametri, dato che argc=%d\n",argc);
        exit(1);
    }   
    N = argc-1;
    /*allocazione memoria dinamica per finito*/
    finito = (int*)malloc(N*sizeof(int));
    if (finito == NULL) {
        printf("Errore nella allocazione della memoria per array finito\n");
        exit(2);
    }
    
    /*inizializzazione a 0 di finito: all'inizio nessun figlio è finito;*/
    memset(finito,0,N*sizeof(int));
    
    /*allocazione dell'array di N pipe descriptors*/
    piped_FP= (pipe_t*)malloc(N*sizeof(pipe_t));
    piped_PF= (pipe_t*)malloc(N*sizeof(pipe_t));
    if (piped_FP == NULL || piped_PF == NULL) {
        printf("Errore: problemi nell'allocazione di memoria per le pipe\n");
        exit(2);
    }

    /*Creazione delle pipe*/
    for(j=0;j<N;j++) {
        if(pipe(piped_FP[j]) < 0 || pipe(piped_PF[j]) < 0) {
            printf("Errore: problemi nella creazione delle pipe\n");
            exit(3);
        }
    }

    printf("DEBUG-Sono il processo padre con pid %d e sto per creare %d figli \n",getpid(),N);
    
    /*Ciclo di generazione dei figli*/
    for(i=0;i<N;i++) {
    
        if((pid = fork()) < 0) {
            printf("Errore nella fork\n");
            exit(4);
        }

        pidFiglio = getpid();
        
        if(pid == 0) {
            /*codice figlio*/
           
           /*chiudiamo pipe che non utilizziamo*/
           for (k=0;k<N;k++) {
            close(piped_FP[k][0]);
            close(piped_PF[k][1]);
            if(i != k) {
                close(piped_FP[k][1]);
                close(piped_PF[k][0]);
            }
           }

           /*apro il file*/
           if ((fd = open(argv[i+1],O_RDONLY))<0) {
             printf("Errore: impossibile aprire il file %s\n",argv[i+1]);
            exit(-1);
           }

            k=0; /*inizialissiamo la variabile k*/
            stampa=0; /*inizialissiamo la variabile stampa*/
            while(read(fd,&linea[k],1)) {
                
                if (linea[k]=='\n') {
                    /*siamo arrivati a fine linea*/
                    
                    /*inviamo al padre il primo carattere della linea corrente*/
                    nw =  write(piped_FP[i][1],&linea[0],1);
                    if( nw != 1) {
                        printf("Errore: scrittura di un numero di byte sbagliati %d\n",nw);
                        exit(-1);
                    }

                    /*leggiamo ciò che ci ha inviato il padre*/
                    nr = read(piped_PF[i][0],&risposta,1);
                    if( nr != 1) {
                        printf("Errore: scrittura di un numero di byte sbagliati %d\n",nw);
                        exit(-1);
                    }

                    if (risposta == 'Y') {
                        /*possiamo stampare*/
                        linea[k]='\0';
                        printf("Il figlio di indice %d avente pid %d stampa il seguente carattere %c della linea %s\n",i,pidFiglio,linea[0],linea);
                        stampa++;
                    } 
                    k=0;
                } else {
                    k++;
                }
            }
            ritorno = stampa;
            exit(ritorno);
        }
    }

    /*codice padre*/
    /*chiudiamo le pipe che non utilizziamo*/
    for (j=0;j<N;j++) {
        close(piped_FP[j][1]);
        close(piped_PF[j][0]);
    }

    while(!finitof()) {

        char max = -1;
        int index = 0;

        for (i=0;i<N;i++) {

            /*leggiamo dalla pipe dal figlio al padre*/
            finito[i]= (read(piped_FP[i][0],&ch,sizeof(char)) != sizeof(char));

            if (!finito[i]) {
                if (ch > max) {
                    /*settiamo il max*/
                    max = ch;
                    index = i;
                }
            }
        }

        /*primo giro fatto*/
        char yes = 'Y';
        char no = 'N';
        
        /*mandiamo il segnale ai figli*/
        for (j=0;j<N;j++) {
            if (!finito[j]) {
                if (j== index) {
                nw = write(piped_PF[j][1],&yes,1);
                    if( nw != 1) {
                        printf("Errore: scrittura di un numero di byte sbagliati %d\n",nw);
                        exit(-1);
                    }
                } else {
                nw = write(piped_PF[j][1],&no,1);
                    if( nw != 1) {
                        printf("Errore: scrittura di un numero di byte sbagliati %d\n",nw);
                        exit(-1);
                    }
                }
            }
        }
    }
   
    /*Il padre aspetta tutti i processi figli*/
    for(j=0;j<N;j++) {
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
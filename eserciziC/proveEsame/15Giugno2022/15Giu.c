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
    int i,j,k;
    char ch;
    int nw,nr;
    int fd;	/*file descriptor per apertura file*/
    int pid;
    int pidFiglio,status,ritorno;	/*per fork e wait*/
    pipe_t *piped_FP;
    pipe_t *piped_PF;
    char controllo;
    int indice_max;

    /*----------------------------------------------*/

    /*controllo sul numero di parametri*/
    if (argc < 3) {
        printf("Errore numero di parametri, dato che argc=%d\n",argc);
        exit(1);
    }
    
    N = argc-1;

    /*allocazione dell'array di N pipe descriptors*/
    piped_FP= (pipe_t*)malloc(N*sizeof(pipe_t));
    piped_PF= (pipe_t*)malloc(N*sizeof(pipe_t));
    if (piped_FP == NULL || piped_PF == NULL) {
        printf("Errore: problemi nell'allocazione di memoria per le pipe\n");
        exit(2);
    }

    for(j=0;j<N;j++) {
        if(pipe(piped_FP[j]) < 0 || pipe(piped_PF[j]) < 0) {
            printf("Errore: problemi nella creazione delle pipe\n");
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
            exit(4);
        }
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
            int stampa = 0;
            k =0; /*numero di stampe effettuate*/
            while(read(fd,&ch,1)) {
                
                if (islower(ch)) {
                
                    /*se ho trovato un carattere minuscolo, lo invio al padre*/
                    nw = write(piped_FP[i][1],&ch,1);
                    if (nw != sizeof(ch)) {
                        printf("Errore: impossibile scrivere sulla pipe\n");
                        exit(-1);
                    }
                    /*leggiamo ciò che ci ha inviato il padre*/
                    nr = read(piped_PF[i][0],&controllo,1);
                    if (nr != sizeof(controllo)) {
                        printf("Errore: impossibile leggere dalla pipe\n");
                        exit(-1);
                    }
                    if (controllo == 'Y') {
                        /*autorizzato a scrivere*/
                        printf("Il figlio di indice %d con pid %d ha trovato il carattere %c nella posizione %d del file %s\n",i,getpid(),ch,k,argv[i+1]);
                        stampa++;
                    } 
                }
                k++;
            }

            exit(stampa);;
        }
    }

    /*codice padre*/
    
    /*chiudiamo le pipe che non utilizziamo*/
    for (j=0;j<N;j++) {
        close(piped_FP[j][1]);
        close(piped_PF[j][0]);
    }
    
   

    /*cicliamo tutti i figli in ordine finchè non hanno finito*/
    while(!finitof()) 
    {
        int max = -1;
        for(i = 0;i<N;i++) 
        {
            /*tentiamo di leggere la posizione del figlio i-esimo: contestualmente viene aggiornato i valore del corrispondente elemento dell'array finito*/
            finito[i]= (read(piped_FP[i][0],&ch,sizeof(ch)) != sizeof(ch));

            if (!finito[i]) {
                /*il processo è ancora vivo*/
                if (ch > max) {
                    //printf("Bebug- trovato max %c, in  indice %d\n",ch,indice_max);
                    max = ch;
                    indice_max = i;
                }
            }
        }
    

        /*dobbiamo dire ai figli se scrivere o meno*/
        for (i=0;i<N;i++) 
        {
            if (i == indice_max) {
                controllo= 'Y';
            } else  {
                controllo = 'N';
            }

            if (!finito[i]) {
                nw = write(piped_PF[i][1],&controllo,1);
                if (nw != sizeof(controllo)) {
                    printf("Il padre non è riuscito ad inviare nulla al figlio di indice %d\n",i);
                    exit(-1);
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
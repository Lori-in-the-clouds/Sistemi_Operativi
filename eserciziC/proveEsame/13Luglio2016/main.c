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
    int fd;	/*file descriptor per apertura file*/
    int pid;	/*pid per fork*/
    int pidFiglio,status,ritorno;	/*per valore di ritorno figli*/
    pipe_t *piped_FP;
    pipe_t *piped_PF;
    int divisore;
    int occ; /*numero di caratteri inviati al padre da ogni singolo figlio*/
    /*----------------------------------------------*/
    
    /*allocazione memoria dinamica per finito*/
    finito = (int*)malloc(N*sizeof(int));
    if (finito == NULL) {
        printf("Errore nella allocazione della memoria per array finito\n");
        exit(1);
    }
    
    /*inizializzazione a 0 di finito: all'inizio nessun figlio è finito;*/
    memset(finito,0,N*sizeof(int));
    
    /*controllo sul numero di parametri*/
    if (argc < 3) {
        printf("Errore numero di parametri, dato che argc=%d\n",argc);
        exit(1);
    }  

    N = argc-1;

    /*contorlliamo che il numero di parametri sia un nuemero pari*/
    if (N %2 == 1) {
        printf("Errore, numero di parametri non pari\n");
        exit(2);
    }
    
    N/=2;

    /*allocazione dell'array di N pipe descriptors*/
    piped_FP= (pipe_t*)malloc(N*sizeof(pipe_t));
    piped_PF= (pipe_t*)malloc(N*sizeof(pipe_t));
    if (piped_FP == NULL || piped_PF == NULL) {
        printf("Errore: problemi nell'allocazione di memoria per le pipe\n");
        exit(3);
    }

    for(j=0;j<N;j++) {
        if(pipe(piped_FP[j]) < 0 || pipe(piped_PF[j]) < 0) {
            printf("Errore: problemi nella creazione delle pipe\n");
            exit(4);
        }
    }


    printf("DEBUG-Sono il processo padre con pid %d e sto per creare %d figli \n",getpid(),N);
    for(i=0;i<N;i++) {
    
        if((pid = fork()) < 0) {
            printf("Errore nella fork\n");
            exit(5);
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
            if ((fd = open(argv[2*i +1],O_RDONLY))<0) {
                 printf("Errore: impossibile aprire il file %s\n",argv[2*i +1]);
                exit(-1);
            }

            /*leggo il divisore inviato dal padre*/
            read(piped_PF[i][0],&divisore,sizeof(divisore));


            k=1;
            occ = 0;
            while(read(fd,&ch,1)) {
                
                if (k % divisore == 0) {
                    /*scriviamo al padre*/
                    write(piped_FP[i][1],&ch,1);
                    occ++;
                }
                k++;
            }
            exit(occ);
        }
    }

    /*codice padre*/

    /*chiudiamo le pipe che non utilizziamo*/
    for (j=0;j<N;j++) {
        close(piped_FP[j][1]);
        close(piped_PF[j][0]);
    }

    int div[N];
    
    for (i=0;i<N;i++) {
        printf("Per il file %s avente una lunghezza in byte di %d,inserire un divisore:\n",argv[2*i +1],atoi(argv[2*i +2]));
        scanf("%d",&div[i]);

        /*controllo parametro della scanf*/
        if (atoi(argv[2*i +2]) % div[i] == 1) {
            printf("Divisore inserito non corretto\n");
            exit(-1);
        }

        /*altrimenti se è corretto scrivaimolo al figlio*/
        write(piped_PF[i][1],&div[i],sizeof(div[i]));
    } 
    
    k=0;
    while(!finitof()) {
        
        for (int i=0;i<N;i++) {
            finito[i]= (read(piped_FP[i][0],&ch,sizeof(ch))!=sizeof(ch));

                
            if (!finito[i]) {                    
                printf("Il figlio di indice %d ha letto nel file %s nella posizione %d il carattere %c\n",i,argv[2*i+1],div[i]*k,ch);
            }
        }
        k++;
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
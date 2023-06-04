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
    int pid;	/*process identifier per le fork()*/
    int pidFiglio,status,ritorno;	/*per valore di ritorno figli*/
    int nr,nw; /*variabili per salvare/controllare valori di ritorno di read/write da/su pipe*/
    pipe_t *piped_PF;
    pipe_t *piped_FP;
    int cur;    /*posizone all'interno del file del carattere ch*/
    char risposta; 
    int stampa; /*numero di stampe effetuate da ogni figlio*/

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

    /*Creazione delle pipe*/
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

    //printf("DEBUG-Sono il processo padre con pid %d e sto per creare %d figli \n",getpid(),N);
    
    /*Ciclo di generazione dei figli*/
    for(i=0;i<N;i++) {
    
        if((pid = fork()) < 0) {
            printf("Errore nella fork\n");
            exit(5);
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

            /*leggiamo il file*/
            stampa=0;
            cur = 0;
            while(read(fd,&ch,1)) {

                /*controlliamo che il carattere sia minuscolo*/
                if (islower(ch)) {

                    /*inviamo al padre tale carattere*/
                    nw = write(piped_FP[i][1],&ch,1);
                    if (nw != 1) {
                        printf("Erroore nella scrittura nella pipe tra figlio-padre\n");
                        exit(-1);
                    }

                    /*aspettiamo la risposta del padre*/
                    nr = read(piped_PF[i][0],&risposta,sizeof(risposta));
                    if (nr != 1) {
                        printf("Errore nella lettura nella pipe tra padre-figlio\n");
                        exit(-1);
                    }
                    
                    if (risposta == 'Y') {
                        printf("Il figlio di indice %d e pid %d alla posizione %d del file %s ha il carattere %c\n",i,pidFiglio,cur,argv[i+1],ch);
                        stampa++;
                    } 
            
                }
                cur++;
            }
            ritorno = stampa;
            exit(stampa);
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
        k=0; /*indice del figlio avente il ch maggiores*/
        
        for (i=0;i<N;i++) {
           
            nr = finito[i]= (read(piped_FP[i][0],&ch,sizeof(ch)) != sizeof(ch));
            
            
            if (!finito[i]) {
                /*preocesso non ancora finito*/
                if (ch > max) {                       
                    max = ch;
                    k=i;
                }
            }
        }

        /*inviamo 'Y' ha chi ha il max invece 'N' ha chi non ha il max*/
        for (i=0;i<N;i++) {

            if (!finito[i]) {
                if (i== k) {
                    char yes ='Y';
                   nw = write(piped_PF[i][1],&yes,1);
                } else {
                    char no ='N';
                   nw = write(piped_PF[i][1],&no,1);
                }

                if (nw != 1) {
                    printf("Errore sull'ultima scrittura della pipe\n");
                    exit(-1);
                } 
            }
        }
    }

    /*Il padre aspetta tutti i processi figli*/
    for(i=0;i<N;i++) {
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
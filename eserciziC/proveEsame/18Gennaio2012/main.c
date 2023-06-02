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
    char C; /*carattere dato come parametr*/
    int K;/*numero intero strettamente positivo dato come parametro*/
    int i,j,k;	/*indici*/
    char ch;
    int fd;	/*file descriptor per apertura file*/
    int pid;	/*pid per fork*/
    int pidFiglio,status,ritorno;	/*per valore di ritorno figli*/
    pipe_t *piped;
    int occ; /*parametro utilizzato per contare le occorrenze del carattere C per ciascuna linea dei file*/
    /*----------------------------------------------*/

    /*controllo sul numero di parametri*/
    if (argc < 4) {
        printf("Errore numero di parametri, dato che argc=%d\n",argc);
        exit(1);
    } 

    N = argc-3;  

    /*Controllo che  sia un singolo carattere*/
    if (strlen(argv[argc-2]) != 1) {
        printf("Errore: parametro passato %s non è un singolo carettere\n",argv[argc-2]);
        exit(2);
    }

    C = argv[argc -2][0];

    /*Controllo che  sia un numero strettamente positivo*/
    if (atoi(argv[argc-1]) < 0) {
        printf("Errore: parametro passato %s non è un numero strettente positivo\n",argv[argc-1]);
        exit(3);
    }

    K = atoi(argv[argc-1]);

    /*allocazione dell'array di N pipe descriptors*/
    piped= (pipe_t*)malloc(N*sizeof(pipe_t));
    if (piped == NULL) {
        printf("Errore: problemi nell'allocazione di memoria per la pipe\n");
        exit(4);
    }

    for(j=0;j<N;j++) {
        if(pipe(piped[j]) < 0) {
            printf("Errore: problemi nella creazione della pipe\n");
            exit(5);
        }
    }

    printf("DEBUG-Sono il processo padre con pid %d e sto per creare %d figli \n",getpid(),N);
    for(i=0;i<N;i++) {
    
        if((pid = fork()) < 0) {
            printf("Errore nella fork\n");
            exit(6);
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

            occ=0;
            ritorno=0;
            /*leggiamo il file associato a tale processo*/
            while(read(fd,&ch,1)) {
                
                if (ch == '\n') {
                    /*scriviamo al padre*/
                    write(piped[i][1],&occ,sizeof(occ));
                    occ = 0;
                }
                if (ch == C) {
                    occ++;
                    ritorno = 1; /*se è stato trovato almeno una volta il carattere dentro il file,allora il figlio ritorna al padre 1, altrimenti 0*/
                }

            }

            exit(ritorno);
        }
    }

    /*codice padre*/

    /*chiudiamo le pipe che non utilizziamo*/
    for (j=0;j<N;j++) {
        close(piped[j][1]);
    }

    /*il padre stampa ciò che ha ricevuto dai figli*/
    for (i=0;i<K;i++) {
        for(j=0;j<N;j++) {
            read(piped[j][0],&occ,sizeof(occ));
            printf("Il figlio associato al file %s riguardo la linea %d ha riportato: %d\n",argv[j+1],i,occ);
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
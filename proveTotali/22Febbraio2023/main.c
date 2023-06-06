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
    char car;
    int fd;	/*file descriptor per apertura file*/
    int pid;	/*process identifier per le fork()*/
    int pidFiglio,status,ritorno;	/*per valore di ritorno figli*/
    pipe_t *piped;
    long int somma; /*variabile che utilizziamo per memorizzare la somma dei caratteri numerici presenti nei file*/
    int count; /*variabile che utilizzo per calcolare il numero di caratteri numerici trovati*/
    /*----------------------------------------------*/
   
    /*controllo sul numero di parametri*/
    if (argc < 3) {
        printf("Errore numero di parametri, dato che argc=%d\n",argc);
        exit(1);
    }   
   
    N=argc-1;

    /*allocazione dell'array di N pipe descriptors*/
    piped= (pipe_t*)malloc(N*sizeof(pipe_t));
    if (piped == NULL) {
        printf("Errore: problemi nell'allocazione di memoria per la pipe\n");
        exit(2);
    }

    /*Creazione delle N pipe*/
    for(j=0;j<N;j++) {
        if(pipe(piped[j]) < 0) {
            printf("Errore: problemi nella creazione della pipe\n");
            exit(3);
        }
    }

    printf("DEBUG-Sono il processo padre con pid %d e sto per creare %d figli \n",getpid(),N);
    
    /*Ciclo di generazione dei figli*/
    for(n=0;n<N;n++) {
    
        if((pid = fork()) < 0) {
            printf("Errore nella fork\n");
            exit(4);
        }
        if(pid == 0) {
            /*codice figlio*/
            
            /*chiudiamo pipe che non utilizziamo*/
            for (k=0;k<N;k++) {
                close(piped[k][0]);
                if(n!= k) close(piped[k][1]);
            }

            /*apro il file*/
            if ((fd = open(argv[n+1],O_RDWR))<0) {
                 printf("Errore: impossibile aprire il file %s\n",argv[n+1]);
                exit(-1);
            }

            /*leggiamo il file*/
            somma=0;/*azzeriamo la somma*/
            count=0; /*azzeriamo la variabile count*/
            while(read(fd,&car,1)) {
                
                if (isdigit(car)) {
                    /*il carattere selezionato è un carattere numerico*/
                    somma+= car - '0';
                    count++;
                }
            }

            /*inviamo al padre attraversol l'utilizzo della pipe la somma calcolata*/
            write(piped[n][1],&somma,sizeof(somma));

            /*siamo al termine dell'esecuzione del figlio, il figlio ritorna al padre il numero di caratteri numerici trovati nel file associato*/
            exit(count);
        }
    }

    /*codice padre*/
    /*chiudiamo le pipe che non utilizziamo*/
    for (j=0;j<N;j++) {
        close(piped[j][1]);
    }

    /*il padre legge i valori inviati dai figli*/
    for (n=0;n<N;n++) {
        read(piped[n][0],&somma,sizeof(somma));
        printf("Il figlio di indice %d associato al file %s ha riportato la somma %ld\n",n,argv[n+1],somma);
    }

    /*Il padre aspetta tutti i processi figli*/
    for(n=0;n<N;n++) {
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
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
    int q,j,k;	/*indici*/
    char ch;
    int fd;	/*file descriptor per apertura file*/
    int pid;	/*process identifier per le fork()*/
    int pidFiglio,status,ritorno;	/*per valore di ritorno figli*/
    int B; /*secondo parametro passato, numero strettamente positivo, corrisponde al numero di processi figli*/
    int L;/*terzo parametro passato, numero strettametne positivo, che rappresenta la dimensione (in caratteri) di F ed  multiplo di B */
    char namefile[10];
    pipe_t *piped;
    char *blocco;
    int fdchiara; /*file descripor del file aperto avente l'estensione .Chiara*/
    /*----------------------------------------------*/

    /*controllo sul numero di parametri*/
    if (argc != 4 ) {
        printf("Errore numero di parametri, dato che argc=%d\n",argc);
        exit(1);
    }   

    /*Controllo che il secondo parametro sia un numero strettamente positivo*/
    if (atoi(argv[argc-2]) < 0) {
        printf("Errore: parametro passato %s non è un numero strettente positivo\n",argv[argc-1]);
        exit(2);
    }

    B= atoi(argv[argc-2]);

    /*Controllo che il terzo sia un numero strettamente positivo*/
    if (atoi(argv[argc-1]) < 0) {
        printf("Errore: parametro passato %s non è un numero strettente positivo\n",argv[argc-1]);
        exit(3);
    }
    
    L=atoi(argv[argc-1]);

    sprintf(namefile,"%s.Chiara",argv[1]);
    
    /*apro il file F*/
    if ((fdchiara = open(namefile,O_WRONLY))<0) {
         printf("Errore: impossibile aprire il file %s\n",namefile);
        exit(4);
    }

    /*allocazione dell'array di N pipe descriptors*/
    piped= (pipe_t*)malloc(B*sizeof(pipe_t));
    if (piped == NULL) {
        printf("Errore: problemi nell'allocazione di memoria per la pipe\n");
        exit(5);
    }

    /*Creazione delle N pipe*/
    for(j=0;j<B;j++) {
        if(pipe(piped[j]) < 0) {
            printf("Errore: problemi nella creazione della pipe\n");
            exit(6);
        }
    }

    //printf("DEBUG-Sono il processo padre con pid %d e sto per creare %d figli \n",getpid(),B);
    
    /*Ciclo di generazione dei figli*/
    for(q=0;q<B;q++) {
    
        if((pid = fork()) < 0) {
            printf("Errore nella fork\n");
            exit(7);
        }
        if(pid == 0) {
            /*codice figlio*/
            /*chiudiamo pipe che non utilizziamo*/
            for (k=0;k<B;k++) {
                close(piped[k][0]);
                if(q != k) close(piped[k][1]);
            }

            /*apro il file*/
            if ((fd = open(argv[1],O_RDONLY))<0) {
                 printf("Errore: impossibile aprire il file %s\n",argv[1]);
                exit(-1);
            }

            /*calcoliamo la dimensione del blocco*/
            int dim_blocco = ((q+1)*L/(B-1)) - ((q*L)/B);
            /*allochiamo l'array blocco*/
            blocco = (char*)malloc(dim_blocco*sizeof(char));
            
            /*leggiamo il file*/
            lseek(fd,((q*L)/B),SEEK_SET);
            read(fd,blocco,sizeof(blocco));
            /*scriviamo al padre*/
            write(piped[q][1],&blocco[strlen(blocco)-1],1);
            exit(dim_blocco);
        }
    }

    /*codice padre*/
    /*chiudiamo le pipe che non utilizziamo*/
    for (j=0;j<B;j++) {
        close(piped[j][1]);
    }

    for (j=0;j<B;j++) {
        read(piped[j][0],&ch,1);
        /*scriviamo tale carattere nel file .Chiara*/
        write(fdchiara,&ch,1);
    }

    /*Il padre aspetta tutti i processi figli*/
    for(j=0;j<B;j++) {
        if((pidFiglio = wait(&status)) < 0) {
            printf("Errore wait\n");
            exit(8);
        }
        if((status & 0xFF) != 0) {
            printf("Figlio con pid %d terminato in modo anomalo\n",pidFiglio);
             exit(9);
        } else {
             ritorno = (int)((status >> 8) & 0xFF);
             printf("Il figlio con pid = %d ha ritornato %d (se 255 problemi!)\n",pidFiglio,ritorno);
         }
    }
    exit(0);




}
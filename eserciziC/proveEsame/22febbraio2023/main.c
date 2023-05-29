#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
typedef int pipe_t[2];

int main(int argc, char **argv) {

    /*--------------*variabili locali*--------------*/
    int N;	/*numero di processi figli*/
    int i,j,k;
    char ch;
    int fd;	/*file descriptor per apertura file*/
    int pid;
    int pidFiglio,status,ritorno;	/*per fork e wait*/
    pipe_t *piped;
    long int sum;
    int count; /*variabile utilizzata per contare il numero di caratteri numerici trovati,garantito che sia minore di 255*/
    /*----------------------------------------------*/

    /*controllo sul numero di parametri*/
    if (argc < 3) {
        printf("Errore numero di parametri, dato che argc=%d\n",argc);
        exit(1);
    }
    N = argc-1;
    /*allocazione dell'array di N pipe descriptors*/
    piped= (pipe_t*)malloc(N*sizeof(pipe_t));
    if (piped == NULL) {
        printf("Errore: problemi nell'allocazione di memoria per la pipe\n");
        exit(2);
    }

    for(j=0;j<N;j++) {
        if(pipe(piped[j]) < 0) {
            printf("Errore: problemi nella creazione della pipe\n");
            exit(3);
        }
    }

    printf("DEBUG-Sono il processo padre con pid %d e sto per creare %d figli \n",getpid(),N);
    for(i=0;i<N;i++) {
    
        if((pid = fork()) < 0) {
            printf("Errore nella fork\n");
            exit(4);
        }
        if(pid == 0) {
            /*codice figlio*/
            /*chiudiamo le pipe che non utilizziamo*/
            for (k=0;k<N;k++) {
                close(piped[k][0]);
                if(i != k) close(piped[k][1]);
            }

            /*apro il file*/
            if ((fd = open(argv[i+1],O_RDONLY))<0) {
                 printf("Errore: impossibile aprire in  il file %s\n",argv[i+1]);
                exit(-1);
            }

            sum = 0L; /*inizializziamo la variabile*/
            count = 0; 
            k = 0; /*indice che mi indica la posizione nel file*/
            while (read(fd,&ch,sizeof(ch))) {
                if (k % 2 == 0) {
                    sum+=atoi(&ch);
                    count++;
                }
                k++;
            }

            /*scriviamo al padre la somma risultante*/
            write(piped[i][1],&sum,sizeof(sum));

            exit(sum);
        }
    }

    /*codice padre*/
    /*chiudiamo le pipe che non utilizziamo*/
    for (j=0;j<N;j++) {
        close(piped[j][1]);
    }

    for (i=0;i<N;i++) {
        /*leggiamo i valori inviati dai figli*/
        read(piped[i][0],&sum,sizeof(sum));
        printf("Il figlio di indice %d ha trovato come somma %ld riferito al file %s\n",i,sum,argv[i+1]);
    }

    /*Il padre aspetta tutti i processi figli*/
    for(j=0;j<N;j++) {
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
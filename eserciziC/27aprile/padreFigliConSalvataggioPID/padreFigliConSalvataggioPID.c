#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <fcntl.h> 
#include <time.h>
#include <sys/wait.h>

//funzione per generare numeri random
int mia_random(int n) {
    int casuale;
    casuale = rand() % n;
    return casuale;
}

int main(int argc, char **argv) {
    int N; //numero strettamente positivo / numero di figli
   
    int i; //indice per i figli
    int pidFiglio,status,ritorno; //per il wait e valore di ritorno figli
    
    //generazione seme per il numero random
    srand(time(NULL));

    if (argc != 2) {
        printf("Errore: numero di parametri passato errato");
        exit(1);
    }

    //controllo che il primo parametro sia un numero e che sia strettamente maggiore di 0 e minore di 255
    N= atoi(argv[1]);
    if (N < 0 || N > 255) {
        printf("Errore: parametro passato è un numero negativo oppure è maggiore/uguale di 255\n");
        exit(2);
    }
 
    //riportiamo il pid del procresso padre e il numero n
    printf("PID del processo padre: %d\n",getpid());
    printf("Numero n: %d\n",N);


    //array dinamico per salvare il PID dei figli
    int* pid = malloc(N*sizeof(int));

    //controlliamo che sia stato creato corretamente l'array
    if (pid == NULL) {
        printf("Errore nella malloc\n");
        exit(4);
    }

    //chiamata ai figli
    for(i=0;i<N;i++) {
        if ((pid[i] = fork()) < 0) {
            printf("Errore nella fork\n");
            exit(3);
        }
        if (pid[i] == 0) {
            //codice del figlio
            printf("PID dell' %d-esimo figlio: %d\n",i,getpid());
            //numero random
            int random = mia_random(100+i);
            exit(random);
        }
    }
    
    //chiamata al padre
    for (i=0;i<N;i++) {
        pidFiglio=wait(&status);
        if (pidFiglio < 0) {
            printf("Errore in wait\n");
            exit(4);
        }
        if ((status & 0xFF) != 0) {
            printf("Figlio con pid %d terminato in modo anomalo\n",pidFiglio);
        } else {
              ritorno = (int)((status >> 8) & 0xFF);
              for(int j=0;j<N;j++) {
                if (pidFiglio==pid[j]) {
                    printf("Il figlio con pid=%d,di indice %d ha ritornato il numero random %d\n",pidFiglio,j,ritorno);
                    break;
                }
              }
              
        }
    }
 
}
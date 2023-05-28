#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <fcntl.h> 
#include <sys/wait.h>
#include <time.h>

int mai_random(int n) {
    int casuale;
    casuale = rand() % n;
    return casuale;
}



int main(int argc, char **argv) {
    int pid;
    int pidfiglio;
    int status;
    int ritorno;
    
    //creazione del seme random
    srand(time(NULL));

    //otteniamo il pid corrente e stampiamolo
    printf("PID del processo padre: %d\n",getpid());
    
    //creiamo un processo figlio e assicuriamoci che la creazione sia andata a buon fine
    if ((pid = fork()) < 0) {
        printf("Errore in fork\n");
        exit(1);
    }

    //processo figlio, restituiamo al padre il numero random
    if (pid == 0) {
        printf("PID del processo figlio: %d\n", getpid());
        printf("PID del proceso padre: %d\n",getppid());

        exit(mai_random(100));
    }
    
    //processo padre, il padre aspetta il figlio
    if ((pidfiglio=wait(&status)) < 0) {
        printf("Errore in wait\n");
        exit(2);
    }
    if (pid != pidfiglio) {
        printf("Il pid della wait non corrisponde al pid della fork!\n");
    }
    //ricorda: statuts è il valore di ritorno
    if ((status & 0xFF) != 0) {
        printf("Figlio terminato in modo involontario");
    } else {
        // selezione dei 8 bit piu significativi
        ritorno= (int)((status >>8) & 0xFF);
        printf("Il figlio con pid=%d ha ritornato %d\n",pidfiglio,ritorno);
        //aspettiamo un altro figlio che non esiste!!!
        if (wait((int *)0) < 0) {
            printf("Errore in wait\n");
            exit(3);
        }
    }
    exit(0);

}
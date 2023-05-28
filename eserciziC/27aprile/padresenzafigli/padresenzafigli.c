#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <fcntl.h> 
#include <sys/wait.h>

int main(int argc, char **argv) {
    //stampiamo il pid del padre
    printf("Stampiamo il PID del padre: %d\n",getpid());
    //il padre aspetta il figlio disinteressandosi del valore della exit del figlio
    if (wait((int *)0) < 0) {
        printf("Errore in wait\n");
        exit(1);
    }
    exit(0);
}
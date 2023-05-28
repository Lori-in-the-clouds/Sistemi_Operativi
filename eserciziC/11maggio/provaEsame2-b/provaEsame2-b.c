#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>




int main(int argc, char **argv) {
    int pid;
    int status,ritorno;
    int N = argc - 2; /*numero di parametri*/
    int fd; 
    int j;
    int piped[2];

    struct {
        char chr;
        long int count;
    } a;
    
    //controllo sul numero di parametri 
    if (argc <= 3) {
        printf("Errore: numero di parametri errato\n");
        exit(1);
    }

    //controlliamo che i restanti parametri siano sincoli caratteri
    for (j =1;j < N; j++) {
        int lenght = strlen(argv[j+1]);
        if (lenght != 1) {
            printf("Errore: parametro passato %s non è un singolo carattere\n",argv[j+1]);
            exit(3);
        }
    }

    /*si crea un pipe*/
    if (pipe(piped) < 0) {
        printf("Errore creazione pipe\n");
        exit(4);
    }

    /*Ciclo di generazione dei figli*/
    for(j=0;j<N;j++) {
        
        if ((pid = fork()) < 0) {
            printf("Errore nella fork\n");
            exit(6);
        }

        /*processo figlio*/
        if (pid == 0) {
            
            /*Chiusura delle pipe non usat nella comunicazione con il padre*/
            close(piped[0]);
            
            //facciamo quello che deve fare il figlio
             a.count = 0L;
             a.chr = *argv[j+2];
            
            //controlliamo che il primo parametro sia un nome di un file
            if ((fd = open(argv[1],O_RDONLY)) < 0) {
                printf("Errore sul primo parametro, non è un nome di un file\n");
                exit(2);
            }
            char ch;
            //leggiamo i file carattere per carattere
            while(read(fd,&ch,1)) {
                if (ch ==  (*argv[j+2])) { //attenzione
                    a.count++;
                }
            }

            write(piped[1],&a,sizeof(a));
            exit(*argv[j+2]);
        }


    }   

    /*codice padre*/
    
    /*il padre chiude il lato della pipe che non usa*/
    close(piped[1]);
    

  
    
    /*il padre recupera tutti i valori interi dai figli*/
    while(read(piped[0],&a,sizeof(a)) > 0) {
         printf("%ld occorrenze del carattere %c per il file %s\n",a.count,a.chr,argv[1]);        
    }
   

    /*Il padre aspetta i figli*/
    for (j=0;j<N;j++) {
        
        if ((pid = wait(&status)) < 0) {
            printf("Errore nella wait\n");
            exit(7);
        }

        if ((status & 0xFF) != 0) {
            printf("Figlio con pid %d terminato in modo anomalo\n",pid);
            exit(8);
        }
        else {
            ritorno = (int)((status >> 8) & 0xFF);
            printf("Il figlio con pid=%d ha ritornato %c\n",pid,ritorno);
        }
    }
    exit(0);
}
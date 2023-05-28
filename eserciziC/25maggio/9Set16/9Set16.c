#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>


typedef int pipe_t[2];
typedef struct {
    char v1; //deve contenere il carattere Ci
    long int v2; //deve contare il numero di occorrenze del carattere Ci
} structer;


int compare(const void *a,const void *b) {

    structer arg1 = *(const structer*)a;
    structer arg2 = *(const structer*)b;
    
    if (arg1.v2 < arg2.v2) return -1;
    if (arg1.v2 > arg2.v2) return 1;
    return 0;
}


int main(int argc, char **argv) {
    int pid[26]; 
    int pidFiglio,status,ritorno;
    int i,j;
    pipe_t pipes[26];
    structer A[26];
    int fd;
    char ch;
    char da_trovare;
    long int occ;
    int nr,nw;

    /*controllo sul numero di parametri*/
    if (argc != 2) {
        printf("Errore numero di parametri, dato che argc=%d\n",argc);
        exit(1);
    }

    /*creazione delle 26 pipe*/
    for(i=0;i<26;i++) {
        if (pipe(pipes[i]) < 0) {
            printf("Errore nella creazione della fork\n");
            exit(2);
        }
    }

    /*creazione processi figli*/
    for (i =0;i<26;i++) {
        
        if ((pid[i]=fork())< 0) {
            printf("Errore creazione figlio\n");
            exit(3);
        }

        if (pid[i]==0) {
            /*processo figlio*/
            
            /*chiusura pipe non utilizzate*/
            for(j=0;j<26;j++) {
                if (j!=i) {
                    close(pipes[j][1]);
                }
                if ((i==0) || (j!= i-1)) {
                    close(pipes[j][0]);
                }
            }

            
            /*apriamo il file*/
            if ((fd = open(argv[1],O_RDONLY)) < 0) {
                printf("Impossibile aprire il file\n");
                exit(-1);
            }

            da_trovare = 'a'+ i;
            printf("DEBUG - carattere da trovare %c\n",da_trovare); //ATTENZIONE
            /*leggiamo dal file le occorenze del carattere*/
            occ = 0;
            while(read(fd,&ch,sizeof(ch))) {
                if (ch == da_trovare) {
                    occ++;
                }
            }

            /*settiamo la struct*/
            if (i==0) {
                A[0].v1 = 'a';
                A[0].v2 = occ;
            } else {
                /*lettura da pipe della struttura per tutti i figli a parte il primo*/
                nr = read(pipes[i-1][0],A,sizeof(A));
                
                if (nr != sizeof(A)) {
                    printf("Errore, il figlio %d ha letto un numero di byte sbalgiati %d\n",i,nr);
                    exit(26+1);
                }

                /*settiamo il nuovo elemento dell'array di struct*/
                A[i].v1 = da_trovare;
                A[i].v2 = occ;
            }
            
            /*tutti i figlio mandano in avanti,l'ultimo figlio manda al padre una struttura*/
            nw = write(pipes[i][1],A,sizeof(A));

            if (nw != sizeof(A)) {
                printf("Errore, il figlio %d ha letto un numero di byte sbalgiati %d\n",i,nr);
                exit(26+2);  
            }
            exit(ch);
        }
    }

        /*codice padre*/
        /*chiusura pipe: tutte meno l'ultima in lettura*/
        for (i=0;i<26;i++) {
            close(pipes[i][1]);
            if (i != 25) close(pipes[i][0]);
        }

        /*il padre deve leggere la singola struttura che gli arriva dall'ultimo figlio*/
        nr = read(pipes[25][0],A,sizeof(A));
        
        /*il padre controlla di aver letto qualcosa*/
        if (nr != sizeof(A)) {
                printf("Errore, il figlio %d ha letto un numero di byte sbalgiati %d\n",i,nr);
                exit(5);
        }

        /*il padre ordina*/
        qsort(A,26,sizeof(structer),compare);

        for (i=0;i<26;i++) {
            printf("-----------------------------------------------------------\n");
            printf("Il figlio con pid %d ha riportato la seguente struttura\n",pid[(A[i].v1-'a')]);
            printf("A[%d].v1 = %c\n",i,A[i].v1);
            printf("A[%d].v2 = %ld\n",i,A[i].v2);
        }
        
        /*il padre aspetta i figli*/
        for (i=0;i<26;i++) {
            
            if ((pidFiglio = wait(&status)) < 0) {
                printf("Errore in wait\n");
                exit(7);
            }

            if ((status & 0xFF) != 0) {
                printf("Figlio con pid %d terminato in modo anomalo\n",pidFiglio);
            } else {
                ritorno = (int)((status >> 8) & 0xFF);
                printf("Il figlio con pid=%d ha ritornato %c (se > di %d problemi)\n",pidFiglio,ritorno,25);
            }
        }
        exit(0);

}
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

    int pidFiglio,pidNipote,status,ritorno;
    int N;
    pipe_t *piped_FP;
    pipe_t *piped_NP;
    int j,k;
    int pid;
    int fdf;
    int fdn;
    char ch;
    long int occurencedigit,occurencechar;



    /*controllo sul numero di parametri*/
    if (argc < 3) {
        printf("Errore numero di parametri, dato che argc=%d\n",argc);
        exit(1);
    }

    /*inizializziamo la variabile N*/
    N = argc -1;


    /*allocazione dell'array di N pipe descriptors*/
    piped_FP = (pipe_t*)malloc(N*sizeof(pipe_t));
    piped_NP = (pipe_t*)malloc(N*sizeof(pipe_t));

    if (piped_FP == NULL || piped_NP == NULL) {
        printf("Errore nella allocazione della memoria\n");
        exit(2);
    }
    
    /*creazione delle N pipe*/
    for (j=0;j<N;j++) {
        if (pipe(piped_FP[j]) <0 || pipe(piped_NP[j]) < 0) {
            printf("Errore nella creazione della pipe\n");
            exit(3);
        }
    }

    printf("DEBUG-Sono il processo padre con pid %d e sto per generare %d figli\n",getpid(),N);

    /*Ciclo di generazione dei figli*/
    for (j=0;j<N;j++) {
        
        if ((pid = fork()) < 0) {
            printf("Errore nella generazione della fork\n");
            exit(4);
        }

        if (pid == 0) {
            /*codice del figlio*/

            /*chiusura delle pipe non usate nella comunicazione con il padre*/
            for (k=0;k<N;k++) {
                close(piped_FP[k][0]);
                if (k != j) close(piped_FP[k][1]);
            }

            /*creazione nipote*/
            if ((pid = fork()) < 0) {
                printf("Errore nella creazione della fork per il nipote\n");
                exit(-1);
            }

            if (pid == 0) {
                /*codice nipote*/

                /*chiudiamo la pipe utilizzata prima dal figlio*/
                close(piped_FP[j][1]);
                /*chiudiamo le pipe non utilizate*/
                for (k=0;k<N;k++) {
                    close(piped_NP[j][0]);
                    if (k != j) close(piped_NP[k][1]);
                }

                /*apriamo il file*/
                if ((fdn = open(argv[j+1],O_RDWR)) < 0) {
                    printf("Impossibile aprire il file\n");
                    exit(-1);
                }

                occurencechar = 0;

                /*leggiamo il file*/
                while(read(fdn,&ch,sizeof(ch))) {
                    if (isalpha(ch)) {
                        occurencechar++;
                        //facciamo andare indietro il pointer di uno e poi scriviamo 
                        lseek(fdn, -1L,1);
                        if (!isupper(ch)) ch-=32;
                        write(fdn,&ch,1);
                    }
                }

                //scriviamo sulla pipe verso il padre
                write(piped_NP[j][1],&occurencechar,sizeof(occurencechar));

                /*chiudiamo il file*/
                close(fdn);

                /*ritoniamo un valore al figlio*/
                exit(occurencechar / 256);
            }

            /*codice figlio*/
            //chisura di tutte li pipe utilizzate dal nipote
           /* for(j=0;j<N;j++) {
                close(piped_NP[j][0]);
                close(piped_NP[j][1]);

            }*/
            /*apriamo il file*/
            if ((fdf = open(argv[j+1],O_RDWR)) < 0) {
                printf("Impossibile aprire il file\n");
                exit(-1);
            }

            //leggiamo il file cercando i caratteri numerici
            occurencedigit =0;
            while(read(fdf,&ch,sizeof(ch))) {
                if (isdigit(ch)) {
                    occurencedigit++;
                    //facciamo andare indietro il pointer di uno e poi scriviamo 
                    lseek(fdf, -1L,1);
                    write(fdf, " ",1);
                }
            }
            
            //scriviamo sulla pipe verso il padre
            write(piped_FP[j][1],&occurencedigit,sizeof(occurencedigit));

            /*chiudiamo il file*/
            close(fdf);

            /*il figlio aspetta il nipote*/
            if ((pidNipote = wait(&status)) < 0) {
                 printf("Errore in wait\n");
                 exit(-1);
            }
            if ((status & 0xFF) != 0) {
                printf("Figlio con pid %d terminato in modo anomalo\n",pidNipote);
            }
            ritorno = (int)((status >> 8) & 0xFF);
            printf("Il nipote di pid %d ha ritornato il valore %d(se 255 errore)\n",pidNipote,ritorno);
            exit(occurencedigit / 256);
        }
    }

    /*codice padre*/

    /*chiudiamo le pipe non necessarie*/
    for (j=0;j<N;j++) {
        close(piped_FP[j][1]);
        close(piped_NP[j][1]);
    }

    for (j=0;j<N;j++) {
        read(piped_FP[j][0],&occurencedigit,sizeof(occurencedigit));
        read(piped_NP[j][0],&occurencechar,sizeof(occurencechar));
        printf("Per quanto riguarda il file %s è stato ritornato %ld %ld\n",argv[j+1],occurencedigit,occurencechar);
    }

    /*il padre aspetta il figlio*/
    for (j=0;j<N;j++) {
        
        if ((pidFiglio = wait(&status)) < 0) {
            printf("Errore nella wait\n");
            exit(8);
        }
         if ((status & 0xFF) != 0) {
            printf("Figlio con pid %d terminato in modo anomalo\n",pidFiglio);
            exit(9);
         }
         ritorno = (int)((status >> 8) & 0xFF);
         printf("Il figlio con pid %d ha ritornato il valore %d(se 255 errore)\n",pidFiglio,ritorno);

    }
    exit(0);

}
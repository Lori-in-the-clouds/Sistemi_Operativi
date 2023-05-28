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
    int N;
    char Cx;
    pipe_t *piped_PF;
    pipe_t *piped_FP;
    int status,ritorno,pid;
    int j,k;
    long int i;
    int fd;
    char ch;
    long int count_mod;
    char scarto;
    int finito;
    int nr;

    /*controllo sul numero di parametri*/
    if (argc < 3) {
        printf("Errore numero di parametri, dato che argc=%d\n",argc);
        exit(1);
    }

    if (strlen(argv[argc-1]) != 1) {
        printf("Errore, ultimo parametro passato non è un singolo carattere");
        exit(2);
    }
    
    /*assegnamo alla variabile N il numero di parametri*/
    N = argc -2;
    
    /*assegnamo alla variabile C il carattere inviato come parametro*/
    Cx = *argv[argc-1];
   
    
    /*allocazione dell'array di N pipe descriptors*/
    piped_PF = (pipe_t*)malloc(N*sizeof(pipe_t));
    piped_FP = (pipe_t*)malloc(N*sizeof(pipe_t));
    
    if (piped_PF == NULL || piped_FP == NULL) {
        printf("Errore nella creazione della pipe\n");
        exit(3);
    }

    /*creazione delle N pipe figli-padre e padre-figlio*/
    for (j=0;j<N;j++) {
        if (pipe(piped_FP[j]) < 0 || pipe(piped_PF[j]) < 0) {
            printf("Errore nella creazione della pipe\n");
            exit(4);
        }
    }

    /*ciclo di generazione dei figli*/
    for (j=0;j<N;j++) {

         if ((pid = fork()) < 0) {
            printf("Errore nella fork\n");
            exit(4);
        }

        if (pid == 0) {
                /*figlio*/

                /*chiusura delle pipe non usate per la comunicazione con il padre*/
                for (k=0;k<N;k++) {
                    close(piped_FP[k][0]);
                    close(piped_PF[k][1]);
                    if (k != j) {
                        close(piped_FP[k][1]);
                        close(piped_PF[k][0]);
                    }

                }

                /*FACCIAMO QUELLO CHE DOBBIAMO FARE*/

                /*apriamo il file*/
                if ((fd = open(argv[j+1],O_RDWR)) < 0) {
                    printf("Errore nella open del file %s\n",argv[j+1]);
                    exit(-1);
                }

                count_mod = 0; 
                i=0;
                while(read(fd,&ch,sizeof(ch))) {
                    if (ch == Cx) {
                        /*inviamo al padre la posizione*/
                        write(piped_FP[j][1],&i,sizeof(i));


                        /*leggiamo l'informazione inviata da parte del padre*/
                        read(piped_PF[j][0],&ch,sizeof(ch));
                        
                        if (ch != '\n') {
                            //facciamo andare indietro il pointer di uno e poi scriviamo 
                            lseek(fd, -1L,1);
                            write(fd,&ch,sizeof(ch));
                            count_mod++;
                        }
                    }
                    i++;
                }
                exit(count_mod);
        }

    }

    /*padre*/

    /*chiudiamo le pipe non neccessarie*/
    for (k=0;k<N;k++) {
        close(piped_FP[k][1]);
        close(piped_PF[k][0]);
    }

    /*il padre recupera le informazioni dai figli: prima in ordine di posizioni e quinid in ordine di indice*/
    finito = 0; /*all'inizio supponiamo che non sia finito nessun figlio*/
    while(!finito) {
        finito = 1; /*mettiamo a uno perchè potrebbero essere finiti tutti i figli*/
        for (j=0;j<N;j++) {
           nr = read(piped_FP[j][0],&i,sizeof(i));
           if (nr != 0) {
                finito = 0;
                printf("Il figlio di indice %d ha inviato la posizione di indice %ld del file %s,con quale carattere devo sostituire tale occorenza?\n",j,i,argv[j+1]);
                /*leggiamo il carattere*/
                read(0,&ch,1);
                if (ch != '\n') read(0,&scarto,1);
                //inviamo il carattere al figlio
                write(piped_PF[j][1],&ch,1);
            }
        }
    }

    /*il padre aspetta il figlio*/
    for (j=0;j<N;j++) {
        if ((pid = wait(&status)) < 0) {
            printf("Errore nella wait\n");
            exit(5);
        }

        if ((status & 0xFF) != 0) {
            printf("Figlio con pid %d terminato in modo anomalo\n",pid);
        }
        else {
            ritorno = (int)((status >> 8) & 0xFF);
                printf("Il figlio con pid=%d ha ritornato %d(se 255 errore)\n",pid,ritorno);        
        }

    }
    exit(0);

}
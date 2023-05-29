#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
typedef int pipe_t[2];


int main (int argc,char **argv) {
    /*--------------*variabili locali*--------------*/
    int N;	
    int i,j,k;
    char ch;
    int fd;	/*file descriptor per apertura file*/
    int pid;
    int fdout;
    int status,ritorno;	/*per fork e wait*/
    pipe_t p_FN; /*pipe figlio nipote*/
    pipe_t p_NF; /*pipe nipote figlio*/
    char ok; /*non importa il suo valore*/
    char namefile[10];


    /*----------------------------------------------*/

    /*controllo sul numero di parametri*/
    if (argc < 3) {
        printf("Errore numero di parametri, dato che argc=%d\n",argc);
        exit(1);
    }

    k=1;
    N = argc-1;

    /*controlliammo che N sia un numero pari*/
    if (N % 2 == 1) {
        printf("Errore, numero di parametri passato non è un numero pari\n");
        exit(2);
    }

    printf("DEBUG-Sono il processo padre con pid %d e sto per creare %d figli \n",getpid(),N/2);
    for(i=0;i<N/2;i++) {
    
        if((pid = fork()) < 0) {
            printf("Errore nella fork\n");
            exit(5);
        }
        if(pid == 0) {
            /*codice figlio*/

             /*il figlio crea la pipe per il nipote*/
            if (pipe(p_FN) < 0) {
                printf("Errore nella creazione della pipe\n");
                exit(-1);
            }

            if (pipe(p_NF) < 0) {
                printf("Errore nella creazione della pipe\n");
                exit(-1);
            }

            /*il figlio crea il proprio file*/
            sprintf(namefile,"merge%d",i);
            printf("Debug- nome del file %s\n",namefile);
            
            if ((fdout = creat(namefile,0644)) < 0) {
                printf("Errrore nella creazione del file %s\n",namefile);
                exit(-1);
            }
            
            
           
            if ((pid = fork()) < 0) {
                printf("Errore nella fork\n");
                exit(6);
            }

            if (pid == 0) {
                /*codice nipote*/

                /*chiudiamo le pipe che non utilizza*/
                close(p_FN[1]);
                close(p_NF[0]);

                /*apriamo il file*/
                if ((fd = open(argv[(N/2)+ i + 1],O_RDONLY))<0) {
                    printf("Errore: impossibile aprire il file %s\n",argv[(N/2)+ i + 1]);
                    exit(-1);
                }

                while(read(fd,&ch,1)) {

                    /*leggiamo l'ok del padre*/
                    read(p_FN[0],&ok,1);
                    /*scriviamo sul file*/
                    write(fdout,&ch,1);
                    /*inviamo l'ok al padre*/
                    write(p_NF[1],&ok,1);
                }
                exit(ch);
            }
            else {

            /*codice figlio*/

            /*chiudiamo le pipe che non utilizza*/
            close(p_FN[0]);
            close(p_NF[1]);

            /*apriamo il file*/
            if ((fd = open(argv[i+1],O_RDONLY))<0) {
                printf("Errore: impossibile aprire in  il file %s\n",argv[i+1]);
                exit(-1);
            }
            
            while(read(fd,&ch,1)) {
               
                if (k == 1) {
                   k=0;
                } else {
                     /*leggiamo l'ok del nipote*/
                    read(p_NF[0],&ok,1);
                }
                    /*scriviamo sul file*/
                    write(fdout,&ch,1);
                    /*mandiamo l'ok al nipote*/
                    write(p_FN[1],&ok,1);
                   
            }

            /*Il figlio aspetta il processo nipote*/
            
                if((pid = wait(&status)) < 0) {
                    printf("Errore wait\n");
                    exit(6);
                }
                if((status & 0xFF) != 0) {
                    printf("Nipote con pid %d terminato in modo anomalo\n",pid);
                     exit(7);
                } else {
                     ritorno = (int)((status >> 8) & 0xFF);
                     //printf("Il Nipote con pid = %d ha ritornato %c (se 255 problemi!)\n",pid,ritorno);
                 }
            
            exit(ritorno);

        }
    }
}

    /*codice padre*/
    /*Il padre aspetta tutti i processi figli*/
    for(j=0;j<N/2;j++) {
        if((pid= wait(&status)) < 0) {
            printf("Errore wait\n");
            exit(8);
        }
        if((status & 0xFF) != 0) {
            printf("Figlio con pid %d terminato in modo anomalo\n",pid);
             exit(9);
        } else {
             ritorno = (int)((status >> 8) & 0xFF);
             printf("Il figlio con pid = %d ha ritornato %d (se 255 problemi!)\n",pid,ritorno);
         }
    }
    exit(0);


}
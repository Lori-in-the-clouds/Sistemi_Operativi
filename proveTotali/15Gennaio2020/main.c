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
    int N;	/*numero di processi figli*/
    int i,j,k;	/*indici*/
    char ch;
    int fd;	/*file descriptor per apertura file*/
    int pid;	/*process identifier per le fork()*/
    int pidFiglio,status,ritorno;	/*per valore di ritorno figli*/
    pipe_t p_FN; /*pipe di comunicazione tra figlio e nipote*/
    pipe_t p_NF; 
    char namefile[10];
    char ok;	/*carattere letto dai figli dalla pipe precedente e scritta su quella successiva*/
    int fcreato; /*file descriptor del file creato dal figlio*/
    /*----------------------------------------------*/

    /*controllo sul numero di parametri*/
    if (argc < 3) {
        printf("Errore numero di parametri, dato che argc=%d\n",argc);
        exit(1);
    }   
    N = argc-1;
    
    /*controllo che la variabile N sia un numero pari*/
    if (N % 2 == 1) {
        printf("Errore: parametro passato %d non è un numero pari\n",N);
        exit(3);
    }

   // printf("DEBUG-Sono il processo padre con pid %d e sto per creare %d figli \n",getpid(),N/2);
    
    /*Ciclo di generazione dei figli*/
    for(i=0;i<N/2;i++) {
        
        if((pid = fork()) < 0) {
            printf("Errore nella fork\n");
            exit(6);
        }
        if(pid == 0) {
            /*codice figlio*/

            /*creo la pipe per il nipote*/
            if (pipe(p_NF)<0 || pipe(p_FN)< 0) {
                printf("Errore nella creazione della pipe per il nipote\n");
                exit(-1);
            }

            /*creiamo il file*/
            sprintf(namefile,"%d.merge",i);

            /*creo un nuovo file*/
            if ((fcreato = creat(namefile,0644)) < 0) {
                printf("Errore nella creazione del file\n");
                exit(-1);
            }

            if((pid = fork()) < 0) {
            printf("Errore nella fork\n");
            exit(6);
            }
           

           if (pid == 0) {
                /*codice nipote*/
                close(p_NF[0]);
                close(p_FN[1]);

                /*apro il file*/
                if ((fd = open(argv[(N/2)+i+1],O_RDONLY))<0) {
                     printf("Errore: impossibile aprire il file %s\n",argv[(N/2)+i+1]);
                    exit(-1);
                }

                while(read(fd,&ch,1)) {
                    
                    /*leggiamo l'ok da parte del figlio*/
                    read(p_FN[0],&ok,1);
                    

                    /*scriviamo il carattere su fout*/
                    write(fcreato,&ch,1);
                    
                    ritorno =  ch; /*potrebbe essere l'ultimo carattere letto*/
                    
                    /*mandiamo l'ok al figlio*/
                    write(p_NF[1],&ok,1);
                
                }
                exit(ritorno);
            }

            /*codice figlio*/
            close(p_NF[1]);
            close(p_FN[0]);

            /*apro il file*/
            if ((fd = open(argv[i+1],O_RDONLY))<0) {
                 printf("Errore: impossibile aprire il file %s\n",argv[i+1]);
                exit(-1);
            }
            k=0;
            while(read(fd,&ch,1)) {
                if (k==1) {
                    
                    /*leggiamo l'ok da parte del nipote*/
                    read(p_NF[0],&ok,1);
                    
                } else k=1;
                
                /*scriviamo il carattere su fout*/
                write(fcreato,&ch,1);
                    
                /*mandiamo l'ok al nipote*/
                write(p_FN[1],&ok,1);
            }

            /*Il figlio deve aspettare il nipote*/
            pid = wait(&status);
            if (pid < 0) {
                printf("Errore in wait \n");
                exit(-1);
            }
            /*la stampa da parte del figlio non è richiesta esplicitamente, ma implicitamnete si capisce che deve essere fatta*/
            if ((status & 0xFF) != 0) {
                printf("Nipote con pid %d terminato in modo anomalo\n",pid);
                exit(-1);
            }
            else {
                exit((int)((status >> 8) & 0xFF));
            }
             
        }
    }

    /*codice padre*/

    /*Il padre aspetta tutti i processi figli*/
    for(j=0;j<N/2;j++) {
        if((pidFiglio = wait(&status)) < 0) {
            printf("Errore wait\n");
            exit(7);
        }
        if((status & 0xFF) != 0) {
            printf("Figlio con pid %d terminato in modo anomalo\n",pidFiglio);
             exit(8);
        } else {
             ritorno = (int)((status >> 8) & 0xFF);
             printf("Il figlio con pid = %d ha ritornato %c (se 255 problemi!)\n",pidFiglio,ritorno);
         }
    }
    exit(0);
}
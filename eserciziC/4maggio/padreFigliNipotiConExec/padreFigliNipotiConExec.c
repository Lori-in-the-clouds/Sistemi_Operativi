#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#define PERM 0644

int main(int argc, char **argv) {
    int i;
    int pid;
    int fwd;
    int N = argc -1; //numero di figli
    int pidFiglio,status,ritorno;

    //controllo sul numero di parametri
    if (N < 3) {
        perror("Errore numero di parametri");
        exit(errno);
    }


    //creo N processi
    for (i=0;i<N;i++) {
        if ((pid=fork()) < 0) {
            perror("Errore nella fork");
            exit(errno);
        }
        if (pid == 0) {
            //processo figlio
          

            //creiamo il nome del nuovo file 
            char *Fout = malloc((strlen(argv[i+1]) + 6)*sizeof(char));
            if (Fout == NULL) {
                perror("Errore nella malloc");
                exit(errno);
            }
            strcpy(Fout,argv[i+1]);
            strcat(Fout,".sort");
            

            //creo il file
            if ((fwd =creat(Fout,PERM)) < 0) {
                perror("Impossibile creare il file");
                exit(errno);
            }


            /* chiudiamo il file creato che il figlio non usa */
			close(fwd);
            
            //creo il processo nipote
            if ((pid = fork()) < 0) {
                perror("Errore nella fork nipote");
                exit(errno);
            }
            if (pid == 0) {
                /* chiudiamo lo standard input dato che la specifica dice che dobbiamo usare il comando filtro sort */
				close(0);
                //apriamo il file associato in sola lettura
                if (open(argv[i+1],O_RDONLY) < 0) {
                    perror("Errore file non esistente");
                    exit(errno);
                }
                close(1);
                if (open(Fout,O_WRONLY) < 0) {
                    perror("Errore file non si riesce ad aprire in scirttura");
                    exit(errno);
                }
                //Nipote
                //execlp("sort","sort",(char*)0);
                execl("/usr/bin/sort","sort",argv[i+1],(char*)0); //ATTENZIONE
                perror("Errore nella exec");
                exit(-1);
            }
            //il figlio aspetta il nipote

            if ((pidFiglio=wait(&status))< 0 ) { //ATTENZIONE
                perror("Errore in wait figlio");
                exit(errno);
            }
            if ((status & 0xFF) != 0) {
                perror("Errore in status figlio");
                exit(errno);
            }
            ritorno = (int)((status >> 8) & 0xFF);
            exit(ritorno);


        }
    }

    //codice padre
   

    for(i=0;i<N;i++) {
        if ((pidFiglio=wait(&status))< 0) {
            perror("Errore nella wait padre");
            exit(errno);
        }

        if ((status & 0xFF) != 0) {
            perror("Errore in status padre");
            exit(errno);
        }
        else {
            ritorno = (int)((status >> 8) & 0xFF);
            printf("Il figlio con pid=%d ha ritornato %d (se 255 problemi!)\n", pidFiglio, ritorno);
        }
        
    
         
               
     }
        
    

    exit(0);
}
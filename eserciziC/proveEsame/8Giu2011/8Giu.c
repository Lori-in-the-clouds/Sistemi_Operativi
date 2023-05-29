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
    char C;
    long int PC;
} structer;
int modified;

void handler(int signo) {
    printf("Stampa di DEBUGGING");
    exit(modified);
}
int main(int argc,char **argv) {
    /*--------------*variabili locali*--------------*/
    int N;	/*numero di processi figli*/
    int i,j;
    long int k;
    char ch;
    int fd;	/*file descriptor per apertura file*/
    int pid;
    int C1,C2;
    int pidFiglio,status,ritorno;	/*per fork e wait*/
    pipe_t *piped; /*pipe tra nipoti e padre*/
    pipe_t p; /*pipe tra figlio e nipote*/
    structer A;
    A.PC = 255;
    /*----------------------------------------------*/

    /*controllo sul numero di parametri*/
    if (argc < 4) {
        printf("Errore numero di parametri, dato che argc=%d\n",argc);
        exit(1);
    }

    N = argc - 3;

    /*Controllo che gli ultimi due parametri siano un singolo carattere*/
    if (strlen(argv[argc-1]) != 1 || strlen(argv[argc-2]) != 1) {
        printf("Errore: ultimi parametri passati non sono un singolo carettere\n");
        exit(2);
    }

    C1 = argv[argc-2][0];
    C2 = argv[argc-1][0];
    
    printf("DEBUG-Caratteri da cercare %c e %c\n",C1,C2);

    /*allocazione dell'array di N pipe descriptors*/
    piped= (pipe_t*)malloc(N*sizeof(pipe_t));
    if (piped == NULL) {
        printf("Errore: problemi nell'allocazione di memoria per la pipe\n");
        exit(3);
    }

    for(j=0;j<N;j++) {
        if(pipe(piped[j]) < 0) {
            printf("Errore: problemi nella creazione della pipe\n");
            exit(4);
        }
    }

    printf("DEBUG-Sono il processo padre con pid %d e sto per creare %d figli \n",getpid(),N);
    for(i=0;i<N;i++) {
    
        if((pid = fork()) < 0) {
            printf("Errore nella fork\n");
            exit(5);
        }
        
        if(pid == 0) {
            /*codice figlio*/
           

            /*creiamo la pipe di comunicazione con il nipote*/
            if (pipe(p) < 0) {
                printf("Errore nella pipe\n");
                exit(-1);
            }

            signal(SIGUSR1,handler); //ATTENZIONE
            if((pid = fork()) < 0) {
                printf("Errore nella fork\n");
                exit(5);
            }

            if (pid == 0) {
                /*codice nipote*/

                for(k=0;k<N;k++) {
                    close(piped[k][0]);
                    if (i != k) close(piped[k][1]);
                }
                /*chiudiamo le pipe che non utilizziamo*/
                close(p[1]);

                /*apriamo il file*/
                if ((fd = open(argv[i+1],O_RDONLY))<0) {
                    printf("Errore: impossibile aprire in  il file %s\n",argv[i+1]);
                    exit(-1);
                }

                modified = 0;
                k=0L; /*indice*/
                while(read(fd,&ch,sizeof(ch))) {
                    if (ch == C2) {

                        /*leggiamo ciò che ci ha inviato il figlio*/
                        read(p[0],&A,sizeof(A));
                        if (k < A.PC) {
                            printf("DEBUG_siamo dentro\n");
                            A.C = C2;
                            A.PC = k;
                            modified++;
                        }

                        /*scriviamo comunque al padre*/
                        write(piped[i][1],&A,sizeof(A));
                    }
                    k++;
                }
                exit(modified);
            }

            /*codice figlio*/

            /*chiudiamo le pipe che non utilzziamo*/
            for (k=0;k<N;j++) {
                close(piped[k][0]);
                close(piped[k][1]);
            }
            close(p[0]);

            /*apriamo il file*/
            if ((fd = open(argv[i+1],O_RDONLY))<0) {
                printf("Errore: impossibile aprire in  il file %s\n",argv[i+1]);
                exit(-1);
            }

            modified = 0;
            k=0L; //indice

            while(read(fd,&ch,sizeof(ch))) {
                if (ch == C1) {
                    printf("DEBUG_il figlio di indice %d ha trovato l'occorenza alla posizione %ld,ora lo invia al figlio\n",i,k);
                    A.PC = k;
                    A.C = C1;
                    modified++;
                    /*scriviamo al nipote*/
                    write(p[1],&A,sizeof(A));
                }
                k++;
            }
            
            sleep(1);
            kill(pid,SIGUSR1); //ATTENZIONE 
            /*Il figlio aspetta il nipote*/
            if((pidFiglio = wait(&status)) < 0) {
                    printf("Errore wait\n");
                    exit(6);
                }
                if((status & 0xFF) != 0) {
                    printf("Figlio con pid %d terminato in modo anomalo\n",pidFiglio);
                     exit(7);
                } else {
                     ritorno = (int)((status >> 8) & 0xFF);
                     printf("Il nipote con pid = %d ha ritornato %d (se 255 problemi!)\n",pidFiglio,ritorno);
                 }
            
            exit(modified);
        }
    }

    /*codice padre*/

    /*il padre chiude le pipe che non utilizza*/
    /*chiudiamo le pipe che non utilizziamo*/
    for (j=0;j<N;j++) {
        close(piped[j][1]);
    }

    /*il padre stampa le informazioni ricevute dal nipote*/
    for(j=0;j<N;j++) {
        read(piped[j][0],&A,sizeof(A));
        printf("Il nipote di indice %d ha ritornato C = %c e PC= %ld\n",j,A.C,A.PC);
    }

    /*Il padre aspetta tutti i processi figli*/
    for(j=0;j<N;j++) {
        if((pidFiglio = wait(&status)) < 0) {
            printf("Errore wait\n");
            exit(8);
        }
        if((status & 0xFF) != 0) {
            printf("Figlio con pid %d terminato in modo anomalo\n",pidFiglio);
             exit(9);
        } else {
             ritorno = (int)((status >> 8) & 0xFF);
             printf("Il figlio con pid = %d ha ritornato %d (se 255 problemi!)\n",pidFiglio,ritorno);
         }
    }
    exit(0);
}
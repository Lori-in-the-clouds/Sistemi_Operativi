#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
typedef int pipe_t[2];
typedef int pipe_t[2];

int main(int argc,char **argv) {
    /*--------------*variabili locali*--------------*/
    int N;	/*numero di processi figli*/
    int i,j,k;
    char ch;
    int fd;	 /*file descriptor per apertura file*/
    int pid;
    int pidFiglio,status,ritorno;	/*per fork e wait*/
    pipe_t *piped;
    pipe_t * pipedNipote;
    long int modified; /*variabile utilizzata per contare le modifiche effettuate*/
    /*----------------------------------------------*/
    
    
    /*controllo sul numero di parametri*/
    if (argc < 3) {
        printf("Errore numero di parametri, dato che argc=%d\n",argc);
        exit(1);
    }
    N = argc-1;

   /*allocazione dell'array di N pipe descriptors*/
   piped= (pipe_t*)malloc(N*sizeof(pipe_t));
   pipedNipote= (pipe_t*)malloc(N*sizeof(pipe_t));


   if (piped == NULL || pipedNipote == NULL) {
        printf("Errore: problemi nell'allocazione di memoria per la pipe\n");
        exit(2);
   }

   for(j=0;j<N;j++) {
        if(pipe(piped[j]) < 0 || pipe(pipedNipote[j]) < 0) {
            printf("Errore: problemi nella creazione della pipe\n");
            exit(3);
        }
   }

  

   printf("DEBUG-Sono il processo padre con pid %d e sto per creare %d figli \n",getpid(),N);
   for(i=0;i<N;i++) {
   
        if((pid = fork()) < 0) {
            printf("Errore nella fork\n");
            exit(4);
        }

        if(pid == 0) {
            /*codice figlio*/

            /*chiudiamo pipe che non utilizziamo*/
            for (k=0;k<N;k++) {
                close(piped[k][0]);
                if(i != k) close(piped[k][1]);
            }

            if ((pid = fork()) < 0) {
                printf("Errore nella fork\n");
                exit(4);
            }
            if (pid == 0) {
                /*codice nipote*/

                /*chiusura della pipe rimasta aperta tra figlio-padre che il nipote non usa*/
                close(piped[i][1]);

                /*chiudiamo pipe che non utilizziamo*/
                for (k=0;k<N;k++) {
                    close(pipedNipote[k][0]);
                    if(i != k) close(pipedNipote[k][1]);
                }
                
                if ((fd = open(argv[i+1],O_RDWR))<0) {
                     printf("Errore: impossibile aprire in  il file %s\n",argv[i+1]);
                    exit(-1);
                }
                modified=0;
                /*cerchimamo le occorenze del file */
                while(read(fd,&ch,sizeof(ch))) {
                    if (islower(ch)) {
                        //facciamo andare indietro il pointer di uno e poi scriviamo ilcarattere maiuscolo
                        lseek(fd, -1L,1);
                        ch-=32;
                        write(fd,&ch,1);
                        modified++;
                    }
                }

                /*scriviamo nella pipe*/
                write(pipedNipote[i][1],&modified,sizeof(modified));
                ritorno = modified / 256;
                exit(ritorno);
            }
            
            
            /*codice figlio*/
            if ((fd = open(argv[i+1],O_RDWR))<0) {
                printf("Errore: impossibile aprire in  il file %s\n",argv[i+1]);
                exit(-1);
            }
            modified=0;

            /*cerchimamo le occorenze del file */
            while(read(fd,&ch,sizeof(ch))) {
                if (isdigit(ch)) {
                    //facciamo andare indietro il pointer di uno e poi scriviamo ilcarattere spazio
                    lseek(fd, -1L,1);
                    char space = ' ';
                    write(fd,&space,1);
                    modified++;
                }
            }

            /*scriviamo nella pipe*/
            write(piped[i][1],&modified,sizeof(modified));

            /*Il figlio aspetta il processo nipote*/
    
            if((pid= wait(&status)) < 0) {
                printf("Errore wait tra figlio e nipote\n");
                exit(-1);
            }
            if((status & 0xFF) != 0) {
                printf("Nipote con pid %d terminato in modo anomalo\n",pid);
                 exit(6);
            } else {
                 ritorno = (int)((status >> 8) & 0xFF);
                 printf("Il Nipote con pid = %d ha ritornato %d (se 255 problemi!)\n",pid,ritorno);
            }
            
            ritorno = modified / 256;
            exit(ritorno);
        }
   }

   /*codice padre*/

    /*chiudiamo le pipe che non utilizziamo*/
    for (j=0;j<N;j++) {
        close(piped[j][1]);
        close(pipedNipote[j][1]);
    }

    for (j=0;j<N;j++) {
        read(piped[j][0],&modified,sizeof(modified));
        printf("Il figlio di indice %d ha operato %ld modifiche nel file %s\n",j,modified,argv[j+1]);
        read(pipedNipote[j][0],&modified,sizeof(modified));
        printf("Il Nipote di indice %d ha operato %ld modifiche nel file %s\n",j,modified,argv[j+1]);
    }

    /*Il padre aspetta tutti i processi figli*/
    for(j=0;j<N;j++) {
        if((pidFiglio = wait(&status)) < 0) {
            printf("Errore wait\n");
            exit(7);
        }
        if((status & 0xFF) != 0) {
            printf("Figlio con pid %d terminato in modo anomalo\n",pidFiglio);
             exit(8);
        } else {
             ritorno = (int)((status >> 8) & 0xFF);
             printf("Il figlio con pid = %d ha ritornato %d (se 255 problemi!)\n",pidFiglio,ritorno);
         }
    }
    exit(0);

}
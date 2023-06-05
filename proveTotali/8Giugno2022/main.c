#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
typedef int pipe_t[2];
#define MGSIZE  3
int main(int argc,char **argv) {
    /*--------------*variabili locali*--------------*/
    int N;	/*numero di processi figli*/
    int i,j,k;	/*indici*/
    int fd;	/*file descriptor per apertura file*/
    int nr,nw;	/*variabili per salvare/controllare valori di ritorno di read/write da/su pipe*/
    int pid;	/*process identifier per le fork()*/
    int pidFiglio,status,ritorno;	/*per valore di ritorno figli*/
    pipe_t p;
    pipe_t *piped_F0_F;
    char linea[MGSIZE];
    char buffer[MGSIZE];
    /*----------------------------------------------*/

    /*controllo sul numero di parametri*/
    if (argc < 4) {
        printf("Errore numero di parametri, dato che argc=%d\n",argc);
        exit(1);
    }   
    N= argc-1;

    /*allocazione dell'array di N pipe descriptors*/
    piped_F0_F= (pipe_t*)malloc((N-1)*sizeof(pipe_t));
    if (piped_F0_F == NULL) {
        printf("Errore: problemi nell'allocazione di memoria per la pipe\n");
        exit(4);
    }

    /*Creazione delle N pipe*/
    for(j=0;j<(N-1);j++) {
        if(pipe(piped_F0_F[j]) < 0) {
            printf("Errore: problemi nella creazione della pipe\n");
            exit(5);
        }
    }

    printf("DEBUG-Sono il processo padre con pid %d e sto per creare %d figli \n",getpid(),N);
    
    /*Ciclo di generazione dei figli*/
    for(i=0;i<N;i++) {
    
        if((pid = fork()) < 0) {
            printf("Errore nella fork\n");
            exit(4);
        }
        if(pid == 0) {
            /*codice figlio*/

            if (i==0) {
                /*chiudiamo pipe che non utilizziamo*/
                for(k=0;k<N-1;k++) {
                    close(piped_F0_F[k][0]);
                }

                /*apro il file*/
                if ((fd = open(argv[1],O_RDONLY))<0) {
                     printf("Errore: impossibile aprire il file %s\n",argv[1]);
                    exit(-1);
                }

                k=0;
                int index = 0;
                /*leggiamo dal file*/
                while(read(fd,&linea[k],1)) {
                    
                    if (linea[k]=='\n') {
                        k++;
                        linea[k]='\0';
                        printf("DEBUG-Stampo la linea %s nel figlio 0\n",linea);
                        /*scriviamo sulla pipe*/
                        nw = write(piped_F0_F[index][1],linea,sizeof(linea));
                        if( nw != sizeof(linea)) {
                            printf("Errore: scrittura di un numero di byte sbagliati %d\n",nw);
                            exit(-1);
                        }
                        k=0;
                        index++;

                    } else {
                        k++;
                    }
                }
                exit(0);

            } else {

                /*creiamo una pipe con il nipote*/
                if ((pipe(p)) < 0) {
                    printf("Errore nella creazione della pipe con il nipote\n");
                    exit(-1);
                }

                if((pid = fork()) < 0) {
                    printf("Errore nella fork\n");
                    exit(4);
                }

                if (pid == 0) {
                    /*processo nipote*/
                    for (k=0;k<N-1;k++) {
                        close(piped_F0_F[k][0]);
                        close(piped_F0_F[k][1]);
                    }

                    /*leggo due volte dalla pipe*/
                    /*leggiamo dalla pipe*/
                    nr = read(p[0],buffer,sizeof(buffer));
                    printf("DEBUG-il nipote ha letto ilfile %s\n",buffer);
                    if( nr != sizeof(buffer)) {
                        printf("Errore: lettura di un numero di byte sbagliati %d\n",nr);
                        exit(-1);
                    }

                    /*leggiamo dalla pipe*/
                    nr = read(p[0],linea,sizeof(linea));
                    if( nr != sizeof(linea)) {
                        printf("Errore: lettura di un numero di byte sbagliati %d\n",nr);
                        exit(-1);
                    }
                    printf("DEBUG-il nipote ha letto il file %s\n",linea);

                    /*inserisci il close error*/
                    close(1);
                    dup(p[1]);
                    printf("DEBUG_sto facendo la diff tra %s e %s\n",buffer,linea);
                    execlp("diff","diff",linea,buffer,(char*)0);

                    /*qui non dovernne mai andarc*/
                    printf("Errore nella exev\n");
                    exit(-1);
                }



                /*codice figlio*/
                close(p[0]);

                /*chiudiamo pipe che non utilizziamo*/
                for(k=0;k<N-1;k++) {
                    close(piped_F0_F[k][1]);
                    if (i-1 != k) close(piped_F0_F[k][0]);
                }

                /*leggiamo cio che ci ha inviato il figlio 0*/
                /*leggiamo dalla pipe*/
                nr = read(piped_F0_F[i-1][0],buffer,sizeof(buffer));
                printf("Il filgio ha letto dal figlio 0 il file %s\n",buffer);
                if( nr != sizeof(buffer)) {
                    printf("Errore: lettura di un numero di byte sbagliati %d\n",nr);
                    exit(-1);
                }

                /*apro il file*/
                if ((fd = open(argv[i+1],O_RDONLY))<0) {
                     printf("Errore: impossibile aprire il file %s\n",argv[i+1]);
                    exit(-1);
                }

                k=0;
                while(read(fd,&linea[k],1)) {
                    if (linea[k]=='\n') {
                        k++;
                        linea[k]='\0';
                        
                        /*scriviamo sulla pipe*/
                        nw = write(p[1],buffer,sizeof(buffer));
                        if( nw != sizeof(buffer)) {
                            printf("Errore: scrittura di un numero di byte sbagliati %d\n",nw);
                            exit(-1);
                        }
                        /*scriviamo sulla pipe*/
                        nw = write(p[1],linea,sizeof(linea));
                        if( nw != sizeof(linea)) {
                            printf("Errore: scrittura di un numero di byte sbagliati %d\n",nw);
                            exit(-1);
                        }
                        k=0;

                        /*Il figlio deve aspettare il nipote*/
                        pid = wait(&status);
                        if (pid < 0) {
                            printf("Errore in wait \n");
                            exit(5);
                        }
                        /*la stampa da parte del figlio non è richiesta esplicitamente, ma implicitamnete si capisce che deve essere fatta*/
                        if ((status & 0xFF) != 0)
                            printf("Nipote con pid %d terminato in modo anomalo\n",pid);
                        else {
                                ritorno=(int)((status >> 8) & 0xFF);

                                if (ritorno == 0) {
                                    printf("I file %s e %s sono uguali\n",buffer,linea);
                                } else {
                                    printf("I file %s e %s non sono uguali\n",buffer,linea);
                                }
                        }
                    } else {
                        k++;
                    }
                }
                exit(0);

            }
        }
    }

    /*codice padre*/
    for (k=0;k<N-1;k++) {
        close(piped_F0_F[k][0]);
        close(piped_F0_F[k][1]);

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

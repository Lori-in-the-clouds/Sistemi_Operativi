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
    int j,n;	/*indici*/
    long int k;
    char ch;
    int fd;	/*file descriptor per apertura file*/
    int nw;	/*variabili per salvare/controllare valori di ritorno di read/write da/su pipe*/
    int nw1,nw2;
    int pid;	/*pid per fork*/
    int pidFiglio,status,ritorno;	/*per valore di ritorno figli*/
    int fdcreato;
    long int L1,L2;
    pipe_t *pari;
    pipe_t *dispari;
    int linea; /*variabile per la linea corrente (pari/dispari) letta dai figli dal proprio file*/
    int nro; /*variabile per il valore massimo della lunghezza delle linee pari/dispari dei file*/
    /*----------------------------------------------*/

    /*controllo sul numero di parametri*/
    if (argc < 2) {
        printf("Errore numero di parametri, dato che argc=%d\n",argc);
        exit(1);
    }  
    N = argc-1;

    /*creo un nuovo file*/
    if ((fdcreato = creat("DIMAIO.log",0644)) < 0) {
        printf("Errore nella creazione del file\n");
        exit(2);
    } 

    /*allocazione dell'array di N pipe descriptors*/
    pari= (pipe_t*)malloc(N*sizeof(pipe_t));
    dispari= (pipe_t*)malloc(N*sizeof(pipe_t));
    if (pari == NULL || dispari == NULL) {
        printf("Errore: problemi nell'allocazione di memoria per le pipe\n");
        exit(3);
    }

    for(j=0;j<N;j++) {
        if(pipe(pari[j]) < 0 || pipe(dispari[j]) < 0) {
            printf("Errore: problemi nella creazione delle pipe\n");
            exit(4);
        }
    }

    printf("DEBUG-Sono il processo padre con pid %d e sto per creare %d figli \n",getpid(),N*2);
    for(n=0;n<N*2;n++) {
    
        if((pid = fork()) < 0) {
            printf("Errore nella fork\n");
            exit(-1);
        }
        if(pid == 0) {
            /*codice figlio*/
            
            if (n % 2 == 0) {
                /*processi pari*/
                
                /*chiudiamo pipe che non utilizziamo*/
                for (k=0;k<N;k++) {
                    close(pari[k][0]);
                    close(dispari[k][1]);
                    close(dispari[k][0]);
                    if(n/2 != k) close(pari[k][1]);
                }

                /*apro il file*/
                if ((fd = open(argv[(n/2)+1],O_RDONLY))<0) {
                     printf("Errore: impossibile aprire il file %s\n",argv[(n/2)+1]);
                    exit(-1);
                }

                linea = 1;
                k= 0L; /*indice che mi stabilisce la lunghezza della linea*/
                nro = 0; /*lunghezza massima di una riga*/
                while(read(fd,&ch,1)) {

                    if (ch == '\n') {
                        
                        k++; /*compreso il terminatore di linea*/
                        if (linea % 2 == 0) {

                            /*scriviamo sulla pipe*/
                            nw = write(pari[n/2][1],&k,sizeof(k));
                            if( nw != sizeof(k)) {
                                printf("Errore: scrittura pari di un numero di byte sbagliati %d\n",nw);
                                exit(-1);
                            }

                            if (k > nro) nro = k;
                        }

                        linea++; /*incrementiamo comunque la linea*/
                        k = 0;

                    } else {
                        k++;
                    }
                }

                ritorno = nro;
                exit(nro);
            } else {
                /*processi dispari*/
                
                /*chiudiamo pipe che non utilizziamo*/
                for (k=0;k<N;k++) {
                    close(dispari[k][0]);
                    close(pari[k][1]);
                    close(pari[k][0]);
                    if(n/2 != k) close(dispari[k][1]);
                }

                /*apro il file*/
                if ((fd = open(argv[(n/2)+1],O_RDONLY))<0) {
                     printf("Errore: impossibile aprire il file %s\n",argv[(n/2)+1]);
                    exit(-1);
                }

                linea = 1;
                k= 0L; /*indice che mi stabilisce la lunghezza della linea*/
                nro = 0; /*lunghezza massima di una riga*/
                while(read(fd,&ch,1)) {

                    if (ch == '\n') {
                        
                        k++; /*compreso il terminatore di linea*/
                        if (linea % 2 == 1) {

                            /*scriviamo sulla pipe*/
                            nw = write(dispari[n/2][1],&k,sizeof(k));
                            if( nw != sizeof(k)) {
                                printf("Errore: scrittura dispari di un numero di byte sbagliati %d\n",nw);
                                exit(-1);
                            }

                            if (k > nro) nro = k;
                        }

                        linea++; /*incrementiamo comunque la linea*/
                        k = 0;

                    } else {
                        k++;
                    }
                }

                ritorno = nro;
                exit(nro);

            }
        }
    }

    /*codice padre*/
    /*chiudiamo le pipe che non utilizziamo*/
    for (j=0;j<N;j++) {
        close(pari[j][1]);
        close(dispari[j][1]);
    }

    /*il padre legge dalle pipe*/
    for (n=0;n<N;n++) {
        printf("La Lunghezza delle linee del file %s sono:\n",argv[n+1]);
        linea=1;
        while(1) {
            nw1 = read(dispari[n][0],&L1,sizeof(L1));
            nw2 = read(pari[n][0],&L2,sizeof(L2));

            if (nw1 != 0) {
                printf("Linea numero %d è lunga %ld\n",linea,L1);
                linea++;
            }

            if (nw2 != 0) {
                printf("Linea numero %d è lunga %ld\n",linea,L2);
                linea++;
            }

            if (nw1 + nw2 == 0) break;
        }
    }


    /*Il padre aspetta tutti i processi figli*/
    char string[255];
    for(n=0;n<N*2;n++) {
        if((pidFiglio = wait(&status)) < 0) {
            printf("Errore wait\n");
            exit(6);
        }
        if((status & 0xFF) != 0) {
            printf("Figlio con pid %d terminato in modo anomalo\n",pidFiglio);
             exit(7);
        } else {
             ritorno = (int)((status >> 8) & 0xFF);
             sprintf(string,"Il figlio con pid = %d ha ritornato %d (se 255 problemi!)\n",pidFiglio,ritorno);
             write(fdcreato,string,sizeof(string));
         }
    }
    exit(0);

}
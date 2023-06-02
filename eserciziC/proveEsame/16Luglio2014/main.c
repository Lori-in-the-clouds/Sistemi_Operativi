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
    int X; /*variabile che mi indica la lughezza in linee dei file*/
    int i,j,k;	/*indici*/
    char ch;
    int fd;	/*file descriptor per apertura file*/
    int nr,nw;	/*variabili per salvare/controllare valori di ritorno di read/write da/su pipe*/
    int pid;	/*pid per fork*/
    int pidFiglio,status,ritorno;	/*per valore di ritorno figli*/
    pipe_t *piped;
    char *primiCar; /*array di char utilizzato per inserire i primi caratteri di ogni linea, da parte dei figli*/
    int linea;
    /*----------------------------------------------*/

    /*controllo sul numero di parametri*/
    if (argc < 3) {
        printf("Errore numero di parametri, dato che argc=%d\n",argc);
        exit(1);
    }   

    N = argc - 2;

    /*allochiamo la memoria per l'array di char*/
    primiCar = (char*)malloc(N*sizeof(char));
    if (primiCar == NULL) {
        printf("Errore nella allocazione della memoria\n");
        exit(1);
    }

    /*Controllo che  sia un numero strettamente positivo*/
    if (atoi(argv[argc-1]) < 0) {
        printf("Errore: parametro passato %s non è un numero strettente positivo\n",argv[argc-1]);
        exit(2);
    }

    X = atoi(argv[argc-1]);

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
            /*chiusura pipes inutilizzate*/
            for (j=0;j<N;j++) {
                if (j!=i)
                    close(piped[j][1]);
                if ((i==0) || (j != i-1))
                    close(piped[j][0]);
            }

            /*apro il file*/
            if ((fd = open(argv[i+1],O_RDONLY))<0) {
                 printf("Errore: impossibile aprire il file %s\n",argv[i+1]);
                exit(-1);
            }
            
            j=0;
            char chprimo;
            while(read(fd,&ch,sizeof(ch))) {

                if (j==0) {
                    /*se è il primo carattre della riga lo salviamo in chprimo*/
                    chprimo = ch;
                }
                if (ch == '\n') {
                    if (i != 0) {
                        
                        /*leggiamo dalla pipe*/
                        nr = read(piped[i-1][0],primiCar,sizeof(primiCar));
                        if( nr != sizeof(primiCar)) {
                            printf("Errore: lettura di un numero di byte sbagliati %d\n",nr);
                            exit(-1);
                        }

                    }
                        primiCar[i]=chprimo;
                        
                        /*scriviamo sulla pipe*/
                        nw = write(piped[i][1],primiCar,sizeof(primiCar));
                        if( nw != sizeof(primiCar)) {
                            printf("Errore: scrittura di un numero di byte sbagliati %d\n",nw);
                            exit(-1);
                        }
                        ritorno = chprimo;
                        j=0;
                } else {
                    j++;
                }
           }
            exit(ritorno);

    }
    }


    /*codice padre*/
    /*chiusura pipe: tutte meno l'ultima in lettura*/
    for (i=0;i<N;i++) {
        close(piped[i][1]);
        if (i != N - 1) close(piped[i][0]);
    }
    linea = 1;
    for (j=0;j<X;j++) {
        
        /*leggiamo dalla pipe*/
        nr = read(piped[N-1][0],primiCar,sizeof(primiCar));
        if( nr != sizeof(primiCar)) {
            printf("Errore: lettura padre di un numero di byte sbagliati %d\n",nr);
            exit(-1);
        }
        printf("Per quanto riguarda la linea %d\n",linea);

        for(i=0;i<N;i++) {
            printf("Carattere %c ricevuto dal il processo di indice %d per il file %s\n",primiCar[i],i,argv[i+1]);
        }
        linea++;

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
             printf("Il figlio con pid = %d ha ritornato %c (se 255 problemi!)\n",pidFiglio,ritorno);
         }
    }
    exit(0);


}
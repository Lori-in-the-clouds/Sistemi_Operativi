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
    int indice; /*indice del processo cha ha troato nel suo file associato il minimo numero di occorenze Cx*/
    int occmin; /*valore di tale minimo*/
    int occtotale; /*conteggio globale ottenuto fino a quel momento*/
} structer;

int main(int argc,char **argv) {
    /*--------------*variabili locali*--------------*/
    int N;	/*numero di processi figli*/
    int i,j;	/*indici*/
    char C1,C2; /*caratteri dati come parametri*/
    char ch;
    int fd;	/*file descriptor per apertura file*/
    int nr,nw;	/*variabili per salvare/controllare valori di ritorno di read/write da/su pipe*/
    int pid;	/*pid per fork*/
    int pidFiglio,status,ritorno;	/*per valore di ritorno figli*/
    pipe_t *piped_1;
    pipe_t *piped_2;
    structer A;
    int occ;
    /*----------------------------------------------*/

    /*controllo sul numero di parametri*/
    if (argc < 7) {
        printf("Errore numero di parametri, dato che argc=%d\n",argc);
        exit(1);
    }   

    N = argc -3;

    /*Controllo che sia un singolo carattere*/
    if (strlen(argv[argc-2]) != 1) {
        printf("Errore: parametro passato %s non è un singolo carettere\n",argv[argc-2]);
        exit(2);
    }

    C1 = argv[argc-2][0];

    /*Controllo che sia un singolo carattere*/
    if (strlen(argv[argc-1]) != 1) {
        printf("Errore: parametro passato %s non è un singolo carettere\n",argv[argc-1]);
        exit(3);
    }

    C2 = argv[argc-1][0];

    /*allocazione dell'array di N pipe descriptors*/
    piped_1= (pipe_t*)malloc(N*sizeof(pipe_t));
    piped_2= (pipe_t*)malloc(N*sizeof(pipe_t));
    if (piped_1 == NULL || piped_2 == NULL) {
        printf("Errore: problemi nell'allocazione di memoria per le pipe\n");
        exit(4);
    }

    for(j=0;j<N;j++) {
        if(pipe(piped_1[j]) < 0 || pipe(piped_2[j]) < 0) {
            printf("Errore: problemi nella creazione delle pipe\n");
            exit(5);
        }
    }

    printf("DEBUG-Sono il processo padre con pid %d e sto per creare %d figli \n",getpid(),N*2);
    for(i=0;i<N*2;i++) {
    
        if((pid = fork()) < 0) {
            printf("Errore nella fork\n");
            exit(6);
        }
        if(pid == 0) {
            /*codice figlio*/
            if (i < N) {
                /*primi processi*/
                
                /*chiusura pipes inutilizzate*/
                for (j=0;j<N;j++) {
                        close(piped_2[j][0]);
                        close(piped_2[j][1]);
                    
                    if (j!=i)
                        close(piped_1[j][1]);
                    if ((i==0) || (j != i-1))
                        close(piped_1[j][0]);
                }

                /*apro il file*/
                if ((fd = open(argv[i+1],O_RDONLY))<0) {
                     printf("Errore: impossibile aprire il file %s\n",argv[i+1]);
                    exit(-1);
                }

                occ = 0;
                /*leggiamo il file e troviamo il numero di occorrenze di C1*/
                while(read(fd,&ch,1)) {
                    if (ch == C1) occ++;
                }

                if (i == 0) {
                    /*creiamo la struct iniziale*/
                    A.indice = 0;
                    A.occmin = occ;
                    A.occtotale = occ;
                } else {

                    /*leggiamo dalla pipe*/
                    nr = read(piped_1[i-1][0],&A,sizeof(A));
                    if( nr != sizeof(A)) {
                        printf("Errore: lettura di un numero di byte sbagliati %d\n",nr);
                        exit(-1);
                    }

                    if (occ < A.occmin) {
                        A.indice = i;
                        A.occmin = occ;
                    }
                    /*incrementiamo comunque l'occtotale*/
                    A.occtotale+=occ;
                }

                /*sccriviamo comunque vero il figli successivo*/
                /*scriviamo sulla pipe*/
                nw = write(piped_1[i][1],&A,sizeof(A));
                if( nw != sizeof(A)) {
                    printf("Errore: scrittura di un numero di byte sbagliati %d\n",nw);
                    exit(-1);
                }

                exit(C1);
            
            } else {
                /*secondi processi*/

                /*chiusura pipes inutilizzate*/
                for (j=0;j<N;j++) {
                        close(piped_1[j][0]);
                        close(piped_1[j][1]);

                    if (j!= i-N)
                        close(piped_2[j][1]);
                    if ((i==N) || (j != i-N-1))
                        close(piped_2[j][0]);
                }

                /*apro il file*/
                if ((fd = open(argv[i- N + 1],O_RDONLY))<0) {
                     printf("Errore: impossibile aprire il file %s\n",argv[i-N+1]);
                    exit(-1);
                }

                occ = 0;
                /*leggiamo il file e troviamo il numero di occorrenze di C1*/
                while(read(fd,&ch,1)) {
                    if (ch == C2) occ++;
                }

                if (i == N) {
                    /*creiamo la struct iniziale*/
                    A.indice = 0;
                    A.occmin = occ;
                    A.occtotale = occ;
                } else {

                    /*leggiamo dalla pipe*/
                    nr = read(piped_2[i-N-1][0],&A,sizeof(A));
                    if( nr != sizeof(A)) {
                        printf("Errore: lettura di un numero di byte sbagliati %d\n",nr);
                        exit(-1);
                    }

                    if (occ < A.occmin) {
                        A.indice = i;
                        A.occmin = occ;
                    }
                    /*incrementiamo comunque l'occtotale*/
                    A.occtotale+=occ;
                }

                /*sccriviamo comunque vero il figli successivo*/
                /*scriviamo sulla pipe*/
                nw = write(piped_2[i-N][1],&A,sizeof(A));
                if( nw != sizeof(A)) {
                    printf("Errore: scrittura di un numero di byte sbagliati %d\n",nw);
                    exit(-1);
                }

                exit(C2);

            }
        }
    }

    /*codice padre*/

  /*chiusura pipe: tutte meno l'ultima in lettura*/
  for (i=0;i<N;i++) {
    close(piped_1[i][1]);
    close(piped_2[i][1]);

    if (i != N - 1) {
        close(piped_1[i][0]);
        close(piped_2[i][0]);

    }
  }

  /*il padre legge la prima pipeline*/
  
        printf("Per quanto riguarda il carattere C1 = %c\n",C1);
        /*leggiamo dalla pipe*/
        nr = read(piped_1[N-1][0],&A,sizeof(A));
        if( nr != sizeof(A)) {
            printf("Errore: lettura di un numero di byte sbagliati %d\n",nr);
            exit(-1);
        }

        printf("Il figlio ha ritornato la seguente struttura: indice->%d occomin->%d occtotale->%d\n",A.indice,A.occmin,A.occtotale);
  

  /*il padre legge la seconda pipeline*/
 
        printf("Per quanto riguarda il carattere C2 = %c\n",C2);
        /*leggiamo dalla pipe*/
        nr = read(piped_2[N-1][0],&A,sizeof(A));
        if( nr != sizeof(A)) {
            printf("Errore: lettura di un numero di byte sbagliati %d\n",nr);
            exit(-1);
        }

        printf("Il figlio ha ritornato la seguente struttura: indice->%d occomin->%d occtotale->%d\n",A.indice,A.occmin,A.occtotale);
    


    /*Il padre aspetta tutti i processi figli*/
    for(j=0;j<N*2;j++) {
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
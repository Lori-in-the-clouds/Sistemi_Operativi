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
    int c1; /*indice d'ordine di un processo*/
    int c2; /*numero di occorrenze del carattere Cx*/
} structer;


int main(int argc,char **argv) {
    /*--------------*variabili locali*--------------*/
    int N;	/*numero di processi figli*/
    int H; /*lunghezza in linee dei file*/
    char Cx;
    int i,j;	/*indici*/
    char ch;
    int fd;	/*file descriptor per apertura file*/
    int nr,nw;	/*variabili per salvare/controllare valori di ritorno di read/write da/su pipe*/
    int pid;	/*pid per fork*/
    int pidFiglio,status,ritorno;	/*per valore di ritorno figli*/
    pipe_t *piped;
    int occ;
    structer A;
    /*----------------------------------------------*/

    /*controllo sul numero di parametri*/
    if (argc < 4) {
        printf("Errore numero di parametri, dato che argc=%d\n",argc);
        exit(1);
    }   

    N = argc -2;

    /*Controllo che  sia un numero strettamente positivo*/
    if (atoi(argv[argc-1]) < 0) {
        printf("Errore: parametro passato %s non è un numero strettente positivo\n",argv[argc-1]);
        exit(2);
    }

    H = atoi(argv[argc-1]);

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

    /*chiedo all'utente il carattere*/
    printf("Inserire un carattere:\n");
    scanf("%c",&Cx);

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

            while(read(fd,&ch,1)) {

                if (ch == Cx) occ++;
                
                if (ch == '\n') {
                    
                    if (i!=0) {
                        
                        /*leggiamo dalla pipe*/
                        nr = read(piped[i-1][0],&A,sizeof(A));
                        if( nr != sizeof(A)) {
                            printf("Errore: lettura di un numero di byte sbagliati %d\n",nr);
                            exit(10);
                        }

                        if (occ < A.c2) {
                            A.c1 = i;
                            A.c2 = occ;
                        }
                    } else {
                        A.c1 = 0;
                        A.c2 = occ;
                    }

                    

                    /*scriviamo sulla pipe*/
                    nw = write(piped[i][1],&A,sizeof(A));
                    if( nw != sizeof(A)) {
                        printf("Errore: scrittura di un numero di byte sbagliati %d\n",nw);
                        exit(10);
                    }
                
                    occ = 0;
                }
            }

            exit(0);
        }
    }

    /*codice padre*/
    /*chiusura pipe: tutte meno l'ultima in lettura*/
    for (i=0;i<N;i++) {
        close(piped[i][1]);
        if (i != N - 1) close(piped[i][0]);
    }

    for (i=0;i<H;i++) {
        /*leggiamo dalla pipe*/
        nr = read(piped[N-1][0],&A,sizeof(A));
        if( nr != sizeof(A)) {
            printf("Errore: lettura di un numero di byte sbagliati %d\n",nr);
            exit(10);
        }

        printf("Il figlio riguardo alla linea numero %d ha riportato la seguente struttura: c1->%d c2->%d\n",i,A.c1,A.c2);
    }

    /*Il padre aspetta tutti i processi figli*/
    for(j=0;j<N;j++) {
        if((pidFiglio = wait(&status)) < 0) {
            printf("Errore wait\n");
            exit(6);
        }
        if((status & 0xFF) != 0) {
            printf("Figlio con pid %d terminato in modo anomalo\n",pidFiglio);
             exit(7);
        } else {
             ritorno = (int)((status >> 8) & 0xFF);
             printf("Il figlio con pid = %d ha ritornato %d (se 10 problemi!)\n",pidFiglio,ritorno);
         }
    }
    exit(0);



}
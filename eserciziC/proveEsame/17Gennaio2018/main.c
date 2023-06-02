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
    int c1; /*indice d'ordine del processo*/
    int c2; /*numero di occorenze del carattere Cx*/
} structer;
int main(int argc,char **argv) {
    /*--------------*variabili locali*--------------*/
    int N;	/*numero di processi figli*/
    char Cx; /*parametro che rappresenta un singolo carattere*/
    int H; /*parametro che rappresenta un numero intero strettamente positivo*/

    int i,j,k;	/*indici*/
    char ch;
    int fd;	/*file descriptor per apertura file*/
    int nr,nw;	/*variabili per salvare/controllare valori di ritorno di read/write da/su pipe*/
    int pid;	/*pid per fork*/
    int pidFiglio,status,ritorno;	/*per valore di ritorno figli*/
    pipe_t *piped;
    pipe_t *piped_PF;
    int occ; /*variabile utilizzata per salvare il numero di occorrenze del carattere Cx*/
    structer A;
    char linea[255];
    /*----------------------------------------------*/

    /*controllo sul numero di parametri*/
    if (argc < 4) {
        printf("Errore numero di parametri, dato che argc=%d\n",argc);
        exit(1);
    }   

    N = argc-3;

    /*Controllo che sia un singolo carattere*/
    if (strlen(argv[argc-2]) != 1) {
        printf("Errore: parametro passato %s non è un singolo carettere\n",argv[argc-2]);
        exit(2);
    }
    
    Cx = argv[argc-2][0];

    /*Controllo che  sia un numero strettamente positivo*/
    if (atoi(argv[argc-1]) < 0) {
        printf("Errore: parametro passato %s non è un numero strettente positivo\n",argv[argc-1]);
        exit(3);
    }

    H = atoi(argv[argc-1]);

    /*allocazione dell'array di N pipe descriptors*/
    piped= (pipe_t*)malloc(N*sizeof(pipe_t));
    piped_PF= (pipe_t*)malloc(N*sizeof(pipe_t));
    if (piped == NULL || piped_PF == NULL) {
        printf("Errore: problemi nell'allocazione di memoria per le pipe\n");
        exit(4);
    }

    for(j=0;j<N;j++) {
        if(pipe(piped[j]) < 0 || pipe(piped_PF[j]) < 0) {
            printf("Errore: problemi nella creazione delle pipe\n");
            exit(5);
        }
    }

    //printf("DEBUG-Sono il processo padre con pid %d e sto per creare %d figli \n",getpid(),N);
    for(i=0;i<N;i++) {
    
        if((pid = fork()) < 0) {
            printf("Errore nella fork\n");
            exit(6);
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

            /*chiudiamo pipe che non utilizziamo*/
            for (k=0;k<N;k++) {
                close(piped_PF[k][1]);
                if(i != k) close(piped_PF[k][0]);
            }

            /*apro il file*/
            if ((fd = open(argv[i+1],O_RDONLY))<0) {
                 printf("Errore: impossibile aprire il file %s\n",argv[i+1]);
                exit(-1);
            }

            k=0;
            int stampe =0;
            while(read(fd,&linea[k],sizeof(linea[k]))) {
                
                if (linea[k] == Cx) occ++;
                
                if (linea[k] == '\n') {

                    k++;
                    linea[k]='\0'; /*aggiungiamo il terminatore di linea*/

                    if (i != 0) {
                        
                        /*leggiamo dalla pipe*/
                        nr = read(piped[i-1][0],&A,sizeof(A));
                        if( nr != sizeof(A)) {
                            printf("Errore: lettura di un numero di byte sbagliati %d\n",nr);
                            exit(-1);
                        }

                        if (occ > A.c2) {
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
                        exit(-1);
                    }
                    occ = 0;

                    /*leggiamo dalla pipe del padre*/
                    nr = read(piped_PF[i][0],&ch,sizeof(ch));
                    if( nr != sizeof(ch)) {
                        printf("Errore: lettura di un numero di byte sbagliati %d\n",nr);
                        exit(-1);
                    }   

                    if (ch == 'Y') {
                        printf("Il figlio di indice %d stampa la seguente riga: %s",i,linea);
                        stampe++;
                    }

                    k=0;
                    continue;
                }
                k++; /*incrementiamo il k*/

            }

                    
              
            exit(stampe);
        }
    }

    /*codice padre*/
    /*chiusura pipe: tutte meno l'ultima in lettura*/
    for (i=0;i<N;i++) {
        close(piped[i][1]);
        if (i != N - 1) close(piped[i][0]);
    }

    /*chiudiamo le pipe che non utilizziamo*/
    for (j=0;j<N;j++) {
        close(piped_PF[j][0]);
    }

    for(i=0;i<H;i++) {
        /*leggiamo dalla pipe*/
        nr = read(piped[N-1][0],&A,sizeof(A));
        //printf("DEBUG- AL padre è arrivata la seguente struttrua riguardo la linea %d c1->%d c2->%d\n",i,A.c1,A.c2);
        if( nr != sizeof(A)) {
            printf("Errore: lettura di un numero di byte sbagliati %d\n",nr);
            exit(-1);
        }

        /*scriviamo ai figli*/
        char yes = 'Y';
        char no = 'N';
        for (j=0;j<N;j++) {
            if (j == A.c1) {
                /*scriviamo sulla pipe*/
                nw = write(piped_PF[j][1],&yes,sizeof(yes));
                if( nw != sizeof(yes)) {
                    printf("Errore: scrittura di un numero di byte sbagliati %d\n",nw);
                    exit(-1);
                }
            } else {
                /*scriviamo sulla pipe*/
                nw = write(piped_PF[j][1],&no,sizeof(no));
                if( nw != sizeof(no)) {
                    printf("Errore: scrittura di un numero di byte sbagliati %d\n",nw);
                    exit(-1);
                }
            }
         }
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
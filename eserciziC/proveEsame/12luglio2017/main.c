#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
#include <time.h>
typedef int pipe_t[2];


int mia_random(int n)
{
    int casuale;
        casuale = rand() % n;
        casuale++;
        return casuale;
}

typedef struct {
    int c1; /*contiene il pid del nipote*/
    int c2; /*numero della linea*/
    char c3[255]; /*una linea può essere al massimo di 255 carattere, da consegna*/
} structer;

int main(int argc,char **argv) {
    /*--------------*variabili locali*--------------*/
    int N;	/*numero di processi figli*/
    int i,j,k;
    int pid;
    int nr;
    int pidFiglio,status,ritorno;	/*per fork e wait*/
    pipe_t *piped;
    pipe_t p; /*pipe di comunicazione tra nipote e figlio*/
    structer A;
    char opzione[5];
    int finito;
    /*----------------------------------------------*/   

    /*controllo sul numero di parametri*/
    if (argc < 3 || (argc % 2 == 0)) {
        printf("Errore numero di parametri, dato che argc=%d\n",argc);
        exit(1);
    }

    N = (argc-1)/2;

    for (i=0;i<argc-1;i++) {
        if ( i % 2 != 0) {
            /*Controllo che  sia un numero strettamente positivo*/
            if (atoi(argv[i+1]) < 0) {
                printf("Errore: parametro passato %s non è un numero strettente positivo\n",argv[i+1]);
                exit(2);
            }
        }
    }

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
            /*chiudiamo pipe che non utilizziamo*/
            for (k=0;k<N;k++) {
                close(piped[k][0]);
                if(i != k) close(piped[k][1]);
            }

            A.c1 = getpid();


            /*creo la pipe del nipote*/
            if (pipe(p) < 0) {
                printf("Errore nella creazione della pipe\n");
                exit(-1);
            }

            /*creo processo nipote*/
            if ((pid = fork()) < 0) {
                printf("Errore nella fork\n");
                exit(6);
            }

            if (pid == 0) {
                /*codice nipote*/

                /*inizializziamo il seme*/
                srand(time(NULL));

                /*chiudo pipe che era rimasta aperta nel processo figlio*/
                close(piped[i][1]);

            
                int random = mia_random(atoi(argv[(i*2)+2]));
                

                /*costruiamo la stringa di opzione per il comando head*/
                sprintf(opzione,"-%d",random);
                printf("DEBUG_stringa di opzione _ %s",opzione);
                
                /*ridirezioniamo lo standard output verso la pipe p*/
                close(1);
                dup(p[1]);
                close(p[1]);
                close(p[0]);
                
                execlp("head","head",opzione,argv[(2*i)+1],(char *)0);

                /*qui non dovrebbe mai andarci*/
                printf("Errore nella exec\n");
                exit(-1);
            }

            /*codice figlio*/

            /*chiudo la pipe del nipote che non utilizzo*/
            close(p[1]);

            /*inserisco il pid nella struct*/
            A.c1 = pid;

            k=0; /*indice per la linea*/
            ritorno = 0; /*indice della linea*/

            while(read(p[0],&A.c3[k],1)) {
               /*inseriamo il terminatore di linea*/
               //int lenght = strlen(A.c3);
               if (A.c3[k] == '\n') {
                    ritorno++;
                    A.c3[k] = '\0';
                    A.c2 = ritorno;
                    /*inviamo al padre la strtuttura*/
                    write(piped[i][1],&A,sizeof(A));
                    k=0;
               } else {
                k++;
               }
            }

            /*Il figlio aspetta il nipote*/
            
                if((pidFiglio = wait(&status)) < 0) {
                    printf("Errore wait\n");
                    exit(7);
                }
                if((status & 0xFF) != 0) {
                    printf("NIpote con pid %d terminato in modo anomalo\n",pidFiglio);
                     exit(8);
                } else {
                    printf("il Nipote con pid %d ha ritornato %d\n",pidFiglio,(int)((status >> 8) & 0xFF));
                    exit(ritorno);
                    
                 }
                exit(ritorno);
        }
    }

    /*codice padre*/
    /*chiudiamo le pipe che non utilizziamo*/
    for (j=0;j<N;j++) {
        close(piped[j][1]);
    }
    
    finito = 0;
    while(!finito) {
        finito = 1; /*potrebbero esser finiti tutti i figli non lo sappiamo*/

        for (i=0;i<N;i++) {
            nr = read(piped[i][0],&A,sizeof(A));
            if (nr != 0) {
                finito = 0;
                printf("Il figlio di indice %d ha ritornato A.c1->%d A.c2->%d A.c3->%s\n",i,A.c1,A.c2,A.c3);
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
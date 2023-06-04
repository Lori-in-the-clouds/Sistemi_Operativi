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
    char chr; /*contiene il carattere AM*/
    long int occ; /*contiene il massimo tra Pocc  e Nocc*/
    char proc; /*contiene F per figlio oppure N per nipote a seconda di quale dei due processi haa trovato massimo numero di occorrenze di occ*/
} structer;

int main(int argc,char **argv) {
    /*--------------*variabili locali*--------------*/
    int N;	/*numero di processi figli*/
    int i,j,k;	/*indici*/
    char ch;
    int fd;	/*file descriptor per apertura file*/
    int pid;	/*pid per fork*/
    int pidFiglio,status,ritorno;	/*per valore di ritorno figli*/
    pipe_t *piped;
    pipe_t p_NF; /*pipe nipote figlio*/
    pipe_t p_FN; /*pipe figli nipote*/
    structer A;
    long int Pocc; /*numero di occorrenze di AM riferito al file del figlio*/
    long int Nocc; /*numero di occorrenze di AM riferito al file del figlio*/
    char AM; /*prima occorrenza di un carattere maiuscolo nel file associato al figlio*/

    /*----------------------------------------------*/

    /*controllo sul numero di parametri*/
    if (argc < 5) {
        printf("Errore numero di parametri, dato che argc=%d\n",argc);
        exit(1);
    }   

    N = (argc-1)/2;

    /*allocazione dell'array di N pipe descriptors*/
    piped= (pipe_t*)malloc(N*sizeof(pipe_t));
    if (piped == NULL) {
        printf("Errore: problemi nell'allocazione di memoria per la pipe\n");
        exit(2);
    }

    for(j=0;j<N;j++) {
        if(pipe(piped[j]) < 0) {
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

            /*creazione della pipe*/
            if (pipe(p_FN) < 0 || pipe(p_NF) < 0) {
                printf("Errore: problemi nella creazione della pipe\n");
                exit(5);
            }

            if((pid = fork()) < 0) {
                printf("Errore nella fork del nipote\n");
                exit(6);
            }

            if (pid == 0) {
                /*codice nipote*/
        
                /*chiudiamo le pipe che non utilizziamo*/
                close(piped[i][1]); /*chiudo la pipe rimasta apertad dal processo figlio*/
                close(p_NF[0]);
                close(p_FN[1]);


                /*apro il file*/
                if ((fd = open(argv[i+1+N],O_RDONLY))<0) {
                     printf("Errore: impossibile aprire il file %s\n",argv[i+1+N]);
                    exit(-1);
                }

                /*leggiamo il carattere da cercare da parte del figlio*/
                read(p_FN[0],&AM,sizeof(AM));


                Nocc=0L; /*azzeriamo il numero di occorrenze di tale carattere nel file*/
                /*cherchiamo l'occorrenza del carattere AM all'interno del file*/
                while(read(fd,&ch,1)) {
                    if (ch == AM) Nocc++;
                }

                /*inviamo al figlio il valore di Nocc*/
                write(p_NF[1],&Nocc,sizeof(Nocc));
                exit(0);
            } else {

                /*codice figlio*/

                /*chiudiamo le pipe che non utilizziamo*/
                close(p_NF[1]);
                close(p_FN[0]);
                

                /*apro il file*/
                if ((fd = open(argv[i+1],O_RDONLY))<0) {
                    printf("Errore: impossibile aprire il file %s\n",argv[i+1]);
                    exit(-1);
                }

                Pocc=0L; /*azzeriamo il numero di occorrenze*/
                while(read(fd,&ch,1)) {
                    if (isupper(ch)) {

                        /*scriviamo al nipote tale carattere*/
                        write(p_FN[1],&ch,1);
                        
                        AM=ch;  /*inizializziamo la variabile AM*/
                        Pocc++; /*incremntiamo la variabile Pocc in quanto abbiamo trovato la prima occorrenza*/
                        break;
                    }
                }

                //se non abbiamo trovato nessuna lettera Maiuscola
                if (Pocc == 0) {
                    printf("Nessuna lettera Maiuscola trovata nel file %s\n",argv[i+1]);
                    exit(-2); /*ritorniamo -2 nel caso in cui il figlio non abbia trovato nessuna lettera maiuscola nel suo file */
                }
                /*continuiamo a cercare il numero di occorrenze di tale carattere*/
                while(read(fd,&ch,1)) {
                    if (ch == AM) Pocc++;
                }

                /*leggiamo il numero di occorrenze di tale carattere proventiene dal nipote*/
                read(p_NF[0],&Nocc,sizeof(Nocc));

                /*settiamo la struct da inviare poi al padre*/
                A.chr = AM;
                if (Pocc >= Nocc) {
                    A.occ = Pocc;
                    A.proc = 'F';
                } else {
                    A.occ = Nocc;
                    A.proc = 'N';
                }

                /*inviamo la struct al padre*/
                write(piped[i][1],&A,sizeof(A));

                /*Il figlio aspetta il nipote*/
                if((pid = wait(&status)) < 0) {
                    printf("Errore figlio wait\n");
                    exit(7);
                }
                if((status & 0xFF) != 0) {
                    printf("Nipote con pid %d terminato in modo anomalo\n",pid);
                    exit(8);
                } else {
                    ritorno = (int)((status >> 8) & 0xFF);
                    printf("Il Nipote con pid = %d ha ritornato %d (se 255 problemi!)\n",pid,ritorno);
                }
                exit(0);
            }
        }   
    }

    /*codice padre*/
    /*chiudiamo le pipe che non utilizziamo*/
    for (j=0;j<N;j++) {
        close(piped[j][1]);
    }
 
    for (i=0;i<N;i++) {
        read(piped[i][0],&A,sizeof(A));
        if (A.proc == 'F') {
            printf("E' stata ripotortata la seguente struttura riferita al file %s: chr->%c occ->%ld proc->%c\n",argv[i+1],A.chr,A.occ,A.proc);
        } else {
            printf("E' stata ripotortata la seguente struttura riferita al file %s: chr->%c occ->%ld proc->%c\n",argv[i+1+N],A.chr,A.occ,A.proc);
        }
    }
    
    /*Il padre aspetta tutti i processi figli*/
    for(j=0;j<N;j++) {
        if((pidFiglio = wait(&status)) < 0) {
            printf("Errore wait\n");
            exit(9);
        }
        if((status & 0xFF) != 0) {
            printf("Figlio con pid %d terminato in modo anomalo\n",pidFiglio);
             exit(10);
        } else {
             ritorno = (int)((status >> 8) & 0xFF);
             printf("Il figlio con pid = %d ha ritornato %d (se 244 il Figlio non ha trovato nessuna lettera Maiuscola nel proprio file)(se 255 problemi!)\n",pidFiglio,ritorno);
         }
    }
    exit(0);
}

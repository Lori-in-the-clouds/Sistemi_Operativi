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
    int c1; /*conteggio caratteri aventi codice ASCII pari*/
    int c2; /*conteggio caratteri aventi codice ASCII dispari*/
} structer;

int main(int argc,char **argv) {
    /*--------------*variabili locali*--------------*/
    int N;	/*numero di processi figli*/
    int i,j,k;	/*indici*/
    char ch;
    int fd;	/*file descriptor per apertura file*/
    int pid;	/*pid per fork*/
    int pidFiglio,status,ritorno;	/*per valore di ritorno figli*/
    pipe_t *pari;
    pipe_t *dispari;
    structer A;
    int num;
    /*----------------------------------------------*/

    N = argc -1;
    /*controllo sul numero di parametri e controllo che N sia pari*/
    if (argc < 3 || (N % 2 == 1)) {
        printf("Errore numero di parametri, dato che argc=%d\n",argc);
        exit(1);
    }

    /*allocazione dell'array di N pipe descriptors*/
    pari= (pipe_t*)malloc((N/2)*sizeof(pipe_t));
    dispari= (pipe_t*)malloc((N/2)*sizeof(pipe_t));
    if (pari == NULL || dispari == NULL) {
        printf("Errore: problemi nell'allocazione di memoria per le pipe\n");
        exit(2);
    }
    for(j=0;j<N/2;j++) {
        if(pipe(pari[j]) < 0 || pipe(dispari[j]) < 0) {
            printf("Errore: problemi nella creazione delle pipe\n");
            exit(3);
        }
    }

    //printf("DEBUG-Sono il processo padre con pid %d e sto per creare %d figli \n",getpid(),N);
    for(i=0;i<N;i++) {
    
        if((pid = fork()) < 0) {
            printf("Errore nella fork\n");
            exit(4);
        }
        if(pid == 0) {
            /*codice figlio*/

            if (i % 2 == 0) {
                /*processo pari*/
                
                /*chiudiamo pipe che non utilizziamo*/
                for (k=0;k<N/2;k++) {
                    close(pari[k][0]);
                    close(dispari[k][1]);
                    close(dispari[k][0]);
                    
                    if(i/2 != k) close(pari[k][1]);
                }

                /*apro il file*/
                if ((fd = open(argv[i+1],O_RDONLY))<0) {
                     printf("Errore: impossibile aprire il file %s\n",argv[i+1]);
                    exit(-1);
                }
                A.c1 =0;
                A.c2 = 0;
                k=0;
                while(read(fd,&ch,sizeof(ch))) {
                    if (k % 2 == 0 && ch!='\n') {
                    
                        /*siamo in posizione pari*/
                        num = ch;
                       
                        if (num % 2 == 0) A.c1++;
                        else A.c2++; 
                    

                    }
                    k++;
                }

                /*inviamo al padre la struct*/
                write(pari[i/2][1],&A,sizeof(A));

                if (A.c1 > A.c2) exit(0);
                else exit(1);

            } 
            if (i % 2 == 1) {
                /*processo dispari*/

                /*chiudiamo pipe che non utilizziamo*/
                for (k=0;k<N/2;k++) {
                    close(dispari[k][0]);
                    close(pari[k][1]);
                    close(pari[k][0]);
                    
                    if(i/2 != k) close(dispari[k][1]);
                }

                /*apro il file*/
                if ((fd = open(argv[i+1],O_RDONLY))<0) {
                     printf("Errore: impossibile aprire il file %s\n",argv[i+1]);
                    exit(-1);
                }

                A.c1 = 0;
                A.c2 = 0;
                k=0;
                while(read(fd,&ch,sizeof(ch))) {
                    if (k % 2 == 1 && ch != '\n') {

                        /*siamo in posizione dispari*/
                        num = ch;
                        
                        if (num % 2 == 1) A.c1++;
                        else A.c2++; 
                        

                    }
                    k++;
                }

                /*inviamo al padre la struct*/
                write(dispari[i/2][1],&A,sizeof(A));

                if (A.c1 > A.c2) exit(0);
                else exit(1);
            }
        }
    }

    /*codice padre*/
    /*chiudiamo le pipe che non utilizziamo*/
    for (j=0;j<N;j++) {
        close(pari[j][1]);
        close(dispari[j][1]);
    }

    /*leggiamo prima tutte le strutture inviate dai figli pari*/
    printf("Stampiamo le strutture inviate dai figli aventi indice pari\n"); /*non richiesto  dalla consegna,inserito per chiarezza*/
    for (i = 0;i<N;i++) {
        
        if (i % 2 == 0) {
            /*leggiamo dalla pipe*/
            read(pari[i/2][0],&A,sizeof(A));
            printf("Il figlio di indice %d ha ritornato la seguente struttura: c1->%d c2->%d\n",i,A.c1,A.c2);
        }
    }

    /*leggiamo tutte le strutture inviate dai figli dispari*/
    printf("Stampiamo le strutture inviate dai figli aventi indice dispari\n"); /*non richiesto  dalla consegna,inserito per chiarezza*/
    for (i = 0;i<N;i++) {
        
        if (i % 2 == 1) {
            /*leggiamo dalla pipe*/
            read(dispari[i/2][0],&A,sizeof(A));
            printf("Il figlio di indice %d ha ritornato la seguente struttura: c1->%d c2->%d\n",i,A.c1,A.c2);
        }
    }

    /*Il padre aspetta tutti i processi figli*/
    for(j=0;j<N;j++) {
        if((pidFiglio = wait(&status)) < 0) {
            printf("Errore wait\n");
            exit(5);
        }
        if((status & 0xFF) != 0) {
            printf("Figlio con pid %d terminato in modo anomalo\n",pidFiglio);
             exit(6);
        } else {
             ritorno = (int)((status >> 8) & 0xFF);
             printf("Il figlio con pid = %d ha ritornato %d (se 255 problemi!)\n",pidFiglio,ritorno);
         }
    }
    exit(0);

    


}
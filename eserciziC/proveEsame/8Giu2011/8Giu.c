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
    char C;
    long int PC;
} structer;
int main(int argc,char **argv) {
    /*--------------*variabili locali*--------------*/
    int N;	/*numero di processi figli*/
    int i,j;
    long int k;
    char ch;
    int fd;	/*file descriptor per apertura file*/
    int pid;
    int C1,C2;
    int pidFiglio,status,ritorno;	/*per fork e wait*/
    pipe_t *piped_FN;
    pipe_t *piped_NP;
    int occ; /*conta le occorenze del carattere in un file*/
    structer A;
    A.PC = 255;
    /*----------------------------------------------*/

    /*controllo sul numero di parametri*/
    if (argc < 3) {
        printf("Errore numero di parametri, dato che argc=%d\n",argc);
        exit(1);
    }

    N = argc -3;

    /*Controllo che gli ultimi due parametri siano un singolo carattere*/
    if (strlen(argv[argc-1]) != 1 || strlen(argv[argc-2]) != 1) {
        printf("Errore: ultimi parametri passati non sono un singolo carettere\n");
        exit(2);
    }

    C1 = argv[argc-2][0];
    C2 = argv[argc-1][0];
    printf("DEBUG- C1 = %c e C2 = %c\n",C1,C2);

    /*allocazione dell'array di N pipe descriptors*/
    piped_FN= (pipe_t*)malloc(N*sizeof(pipe_t));
    piped_NP= (pipe_t*)malloc(N*sizeof(pipe_t));
    if (piped_FN == NULL || piped_NP == NULL) {
        printf("Errore: problemi nell'allocazione di memoria per le pipe\n");
        exit(3);
    }

    for(j=0;j<N;j++) {
        if(pipe(piped_FN[j]) < 0 || piped_NP[j] < 0) {
            printf("Errore: problemi nella creazione delle pipe\n");
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
            

            if ((pid = fork()) < 0) {
                  printf("Errore nella fork\n");
                  exit(5);
            }

            if (pid == 0) {
                
                /*codice nipote*/
                /*chiudiamo la pipe rimasta aperta del figlio-padre ma che il nipote non usa*/
                for(k=0;k<N;k++) {
                    close(piped_FN[k][1]);
                    if (i != k) {
                        close(piped_FN[k][0]);
                    } 
                }

                /*chiudiamo pipe che non utilizziamo*/
                for (k=0;k<N;k++) {
                    close(piped_NP[k][0]);
                    if(i != k) close(piped_NP[k][1]);
                }

                if ((fd = open(argv[i+1],O_RDONLY))<0) {
                     printf("Errore: impossibile aprire in  il file %s\n",argv[i+1]);
                    exit(-1);
                }

                occ = 0;
                /*leggiamo il file*/
                k=0L;
                while(read(fd,&ch,sizeof(ch))) {
                    if (ch == C2) {
                        occ++;
                        /*leggiamo ciò che ci ha inviato il figlio*/
                        read(piped_FN[i][0],&A,sizeof(structer));
                        printf("debug-Cio che è stato invato dal figlio %ld\n",A.PC);
                        printf("debug-Valore di k = %ld\n",k);
                        if (k < A.PC) {
                            printf("siamo entrati\n");
                            A.C = C2;
                            A.PC = k;
                            printf("DEBUG_Struct A: A->C = %c e A->PC = %ld\n",A.C,A.PC);


                            /*scriviamo sulla pipe verso il padre*/
                            write(piped_NP[i][1],&A,sizeof(structer));
                        } else {
                            write(piped_NP[i][1],&A,sizeof(structer));
                        }
                    }
                    k++;
                }
                exit(occ);
            }

            /*codice figlio*/
            
            /*chiudiamo pipe che non utilizziamo*/
            for (k=0;k<N;k++) {
                close(piped_FN[k][0]);

                if(i != k) {
                    close(piped_FN[k][1]);
                }
            }
            /*chiudiamo le altre pipe*/
            for (k=0;k<N;k++) {
                close(piped_NP[k][0]);
                close(piped_NP[k][1]);
            }
            
            if ((fd = open(argv[i+1],O_RDONLY))<0) {
                    printf("Errore: impossibile aprire in  il file %s\n",argv[i+1]);
                    exit(-1);
            }

            occ = 0;
            /*leggiamo il file*/
            k=0L;
            while(read(fd,&ch,sizeof(ch))) {
                if (ch == C1) {
                    printf("DEBUG_ occorrenza del carattere %c trovata all'indice %ld\n",C1,k);
                    A.C = C1;
                    A.PC = k;
                    printf("DEBUG_Struct A: A->C = %c e A->PC = %ld\n",A.C,A.PC);
                    occ++;
                    write(piped_FN[i][1],&A,sizeof(structer));
                }
                k++;
            }
            
            /*Il figlio aspetta tutti i processi nipoti*/
            
                if((pid= wait(&status)) < 0) {
                    printf("Errore wait\n");
                    exit(6);
                }
                if((status & 0xFF) != 0) {
                    printf("Nipote con pid %d terminato in modo anomalo\n",pid);
                     exit(7);
                } else {
                     ritorno = (int)((status >> 8) & 0xFF);
                     printf("Il nipote con pid = %d ha ritornato il numero di occorrenze %d (se 255 problemi!)\n",pid,ritorno);
                 }
            
            exit(occ);
    
        }
    }

    /*codice padre*/

    /*il padre chiude i processi che non utilizza*/
    /*chiudiamo pipe che non utilizziamo*/
    for (k=0;k<N;k++) {
        close(piped_FN[k][0]);
        close(piped_FN[k][1]);
        close(piped_NP[k][1]);
    }

    /*il padre legge dalla pipe*/
    for(i=0;i<N;i++) {
       read(piped_NP[i][0],&A,sizeof(structer));
       printf("DEBUG_Struct in padre A: A->C = %c e A->PC = %ld\n",A.C,A.PC);

       printf("Nipote di indice %d ha inviato C=%c e PC=%ld\n",i,A.C,A.PC);
    }

    /*Il padre aspetta tutti i processi figli*/
    for(j=0;j<N;j++) {
        if((pidFiglio = wait(&status)) < 0) {
            printf("Errore wait\n");
            exit(-1);
        }
        if((status & 0xFF) != 0) {
            printf("Figlio con pid %d terminato in modo anomalo\n",pidFiglio);
             exit(9);
        } else {
             ritorno = (int)((status >> 8) & 0xFF);
             printf("Il figlio con pid = %d ha ritornato il numero di occorenze %d (se 255 problemi!)\n",pidFiglio,ritorno);
         }
    }
    exit(0);


}
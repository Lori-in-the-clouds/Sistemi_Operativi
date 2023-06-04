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
    int n,j,k;	/*indici*/
    char car;
    int fd;	/*file descriptor per apertura file*/
    int nr,nw;	/*variabili per salvare/controllare valori di ritorno di read/write da/su pipe*/
    int pid;	/*process identifier per le fork()*/
    int pidFiglio,status,ritorno;	/*per valore di ritorno figli*/
    pipe_t *piped;	/*array dinamico di pipe descriptors per comunicazioni figli-padre*/
    int pos; /*parametro che utilizzo per capire se un carattere si trovi in posizione dispari o meno*/
    int fcreato; /*file descripor del nuovo file creato*/
    char *cur; /*array di caratteri*/
    /*----------------------------------------------*/

    /*controllo sul numero di parametri*/
    if (argc < 3) {
        printf("Errore numero di parametri, dato che argc=%d\n",argc);
        exit(1);
    }  

    N = argc-1; 

    /*creo un nuovo file*/
    if ((fcreato = creat("DIMAIO",0644)) < 0) {
        printf("Errore nella creazione del file\n");
        exit(2);
    }

    /*allocazione dell'array di N char */
    cur = (char*)malloc(N*sizeof(char));
    if (cur == NULL) {
        printf("Errore: problemi nell'allocazione di memoria per l'array cur\n");
        exit(3);
    }

    /*allocazione dell'array di N pipe descriptors*/
    piped= (pipe_t*)malloc(N*sizeof(pipe_t));
    if (piped == NULL) {
        printf("Errore: problemi nell'allocazione di memoria per la pipe\n");
        exit(4);
    }

    /*Creazione delle N pipe*/
    for(j=0;j<N;j++) {
        if(pipe(piped[j]) < 0) {
            printf("Errore: problemi nella creazione della pipe\n");
            exit(5);
        }
    }

    //printf("DEBUG-Sono il processo padre con pid %d e sto per creare %d figli \n",getpid(),N);
    
    /*Ciclo di generazione dei figli*/
    for(n=0;n<N;n++) {
    
        if((pid = fork()) < 0) {
            printf("Errore nella fork\n");
            exit(6);
        }
        if(pid == 0) {
            /*codice figlio*/
            
            /*chiusura pipes inutilizzate*/
            for (j=0;j<N;j++) {
                if (j!=n)
                    close(piped[j][1]);
                if ((n==0) || (j != n-1))
                    close(piped[j][0]);
            }

            /*apro il file*/
            if ((fd = open(argv[n+1],O_RDONLY))<0) {
                 printf("Errore: impossibile aprire il file %s\n",argv[n+1]);
                exit(-1);
            }


            pos = 1;
            while(read(fd,&car,1)) {
                
                if (pos % 2 == 1) {
                
                    /*siamo in posizione dispari*/

                    ritorno = car; /*potrebbe essere l'ultimo carattere dispari letto dal padre*/

                    if (n!=0) {
                        
                        /*leggiamo dalla pipe del figlio precedente*/
                        nr = read(piped[n-1][0],cur,sizeof(cur));
                        if( nr != sizeof(cur)) {
                            printf("Errore: lettura di un numero di byte sbagliati %d\n",nr);
                            exit(-1);
                        }
                        /*inseriamo il carattere trovato all'interno dell'array*/
                        cur[n]=car;
                    
                    } else {
                        
                        cur[0]=car;
                    }

                    /*scriviamo sulla pipe verso figlio successivo o padre*/
                    nw = write(piped[n][1],cur,sizeof(cur));
                    if( nw != sizeof(cur)) {
                        printf("Errore: scrittura di un numero di byte sbagliati %d\n",nw);
                        exit(-1);
                    }
                }
                
                pos++; /*incrementiamo la variabile pos*/
            }

            exit(ritorno);

        }
    }

    /*chiusura pipe: tutte meno l'ultima in lettura*/
    for (n=0;n<N;n++) {
        close(piped[n][1]);
        if (n != N - 1) close(piped[n][0]);
    }

    pos=1;

    while(1) {
        /*leggiamo dalla pipe*/
        nr = read(piped[N-1][0],cur,sizeof(cur));
        
        if( nr != sizeof(cur)) break;

        printf("Array %d\n",pos);
        for(k=0;k<N-1;k++) {
            printf("%c ",cur[k]);
        }
            printf("%c\n",cur[N-1]);
        
        pos++;
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
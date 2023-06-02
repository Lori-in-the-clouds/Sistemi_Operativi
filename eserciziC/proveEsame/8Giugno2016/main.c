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

int main(int argc,char **argv) {
    /*--------------*variabili locali*--------------*/
    int N;	/*numero di processi figli*/
    int H;
    int i,j,k;	/*indici*/
    int fd;	/*file descriptor per apertura file*/
    int pid;	/*pid per fork*/
    int pidFiglio,status,ritorno;	/*per valore di ritorno figli*/
    int fdcreato; /*file descriptor per il nuovo file creato*/
    pipe_t *piped_PF;
    pipe_t *piped_FP;
    char linea[255];
    int pos;
    int stampe; /*indice che mi conta quante stampe di caratteri ha fatto il processo sul file fdcreato*/
    /*----------------------------------------------*/

    /*controllo sul numero di parametri*/
    if (argc < 6) {
        printf("Errore numero di parametri, dato che argc=%d\n",argc);
        exit(1);
    }   
 
    N = argc-2;

    /*Controllo che  sia un numero strettamente positivo*/
    if (atoi(argv[argc-1]) < 0) {
        printf("Errore: parametro passato %s non è un numero strettente positivo\n",argv[argc-1]);
        exit(2);
    }
    
    H = atoi(argv[argc-1]);

    /*inizializziamo il seme per la generazione di numeri random*/
    srand(time(NULL));

    /*creo un nuovo file*/
    if ((fdcreato = creat("/tmp/creato",0644)) < 0) {
        printf("Errore nella creazione del file\n");
        exit(3);
    }

    /*allocazione dell'array di N pipe descriptors*/
    piped_FP= (pipe_t*)malloc(N*sizeof(pipe_t));
    piped_PF= (pipe_t*)malloc(N*sizeof(pipe_t));
    if (piped_FP == NULL || piped_PF == NULL) {
        printf("Errore: problemi nell'allocazione di memoria per le pipe\n");
        exit(4);
    }

    for(j=0;j<N;j++) {
        if(pipe(piped_FP[j]) < 0 || pipe(piped_PF[j]) < 0) {
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
            
            /*chiudiamo pipe che non utilizziamo*/
            for (k=0;k<N;k++) {
                close(piped_FP[k][0]);
                close(piped_PF[k][1]);
                if(i != k) {
                    close(piped_FP[k][1]);
                    close(piped_PF[k][0]);
                }
            }

            /*apro il file*/
            if ((fd = open(argv[i+1],O_RDONLY))<0) {
                 printf("Errore: impossibile aprire il file %s\n",argv[i+1]);
                exit(-1);
            }

            k=0; /*indice di linea*/
            stampe = 0;
            while(read(fd,&linea[k],sizeof(char))) {

                if (linea[k] == '\n') {
                    
                    k++; /*contiamo anche il terminatore di linea*/

                    /*scriviamo al padre*/
                    write(piped_FP[i][1],&k,sizeof(k));

                    /*leggiamo cio che ci dice il padre*/
                    read(piped_PF[i][0],&pos,sizeof(pos));

                    /*controlliamo se l'indice che ci indica il padre è ammissibile*/
                    if (pos < k) {
                        /*scriviamo tale carattere sul file fdcreato*/
                        //printf("DEBUG_Il figlio %d scrive\n",i);
                        write(fdcreato,&linea[pos],1);
                        stampe++;
                        
                    } else {
                        ;; /*non facciamo niente*/
                    }
                    
                    k=0;
                
                } else {
                    k++;
                }
            }
            exit(stampe);
        }
    }

    /*codice padre*/

    /*chiudiamo le pipe che non utilizziamo*/
    for (j=0;j<N;j++) {
        close(piped_FP[j][1]);
        close(piped_PF[j][0]);
    }

    int random;
    for(i=0;i<H;i++) {
        
        /*individuiamo in modo random quale lunghezza considerare*/
        random = mia_random(N-1);
        //printf("DEBUG_ per la linea numero %d consideriamo la lunghezza del %d-esimo\n",i,random);


        for (j=0;j<N;j++) {
            
            if (j== random) {
                /*leggiamo la lughezza presa in esame dal numero random*/
                read(piped_FP[random][0],&k,sizeof(k));

                /*calcoliamo nuovamente un numero random*/
                random = mia_random(k);
                //printf("DEBUG_per tale riga è stato generata la posizione %d\n",random);

                for (k=0;k<N;k++) {

                    /*scriviamo a tutti i figli*/
                    write(piped_PF[k][1],&random,sizeof(random));
                }
            }
            else read(piped_FP[j][0],&k,sizeof(k)); /*leggiamo a vuoto le alte pipe, solo per scorrerle*/

            

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
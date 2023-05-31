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
    int i,j,k;	/*indici*/
    char ch,ch1;
    int fd;	/*file descriptor per apertura file*/
    int nr,nw;	/*variabili per salvare/controllare valori di ritorno di read/write da/su pipe*/
    int pid;	/*pid per fork*/
    int pidFiglio,status,ritorno;	/*per valore di ritorno figli*/
    pipe_t *piped; /*comunicazione dal processo pari al processo dispari*/
    int fdcreato;
    /*----------------------------------------------*/

    /*controllo sul numero di parametri*/
    if (argc < 3) {
        printf("Errore numero di parametri, dato che argc=%d\n",argc);
        exit(1);
    }
    N = argc-1;

    /*allocazione dell'array di N pipe descriptors*/
    piped= (pipe_t*)malloc((N/2)*sizeof(pipe_t));
    if (piped == NULL) {
        printf("Errore: problemi nell'allocazione di memoria per la pipe\n");
        exit(2);
    }

    for(j=0;j<N/2;j++) {
        if(pipe(piped[j]) < 0) {
            printf("Errore: problemi nella creazione della pipe\n");
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

            /*apro il file*/
            if ((fd = open(argv[i+1],O_RDONLY))<0) {
                printf("Errore: impossibile aprire il file %s\n",argv[i+1]);
                exit(-1);
            }
            
            if (i % 2 == 1) {
                /*processo dispari*/

                /*chiudiamo pipe che non utilizziamo*/
                for (k=0;k<N/2;k++) {
                    close(piped[k][1]);
                    if(k != i/2) close(piped[k][0]);
                }

                char *namefile = argv[i+1];
                strcat(namefile,".MAGGIORE");
                /*creo il nuovo file*/
                if ((fdcreato = creat(namefile,0644)) < 0) {
                    printf("Errore nella creazione del file\n");
                    exit(5);
                }
               
                k= 0; /*numero di caratteri letti*/
                while(read(fd,&ch,1)) {
                    k++;
                    /*legge cio che ha ricevuto dal processo pari*/
                    nr = read(piped[i/2][0],&ch1,1);
        
                    if( nr != sizeof(ch1)) {
                        printf("Errore: lettura di un numero di byte sbagliati %d\n",nr);
                        exit(-1);
                    }
                    
                    if (ch > ch1) {
                        write(fdcreato,&ch,1);
                    } else {
                        write(fdcreato,&ch1,1);
                    }
                }
                exit(k);
            } else {
                /*processi pari*/

                /*chiudiamo pipe che non utilizziamo*/
                for (k=0;k<N/2;k++) {
                    close(piped[k][0]);
                    if(k != i/2) close(piped[k][1]);
                }

                

                k= 0; /*numero di caratteri letti*/
                while(read(fd,&ch,1)) {
                    k++;
                    /*scriviamo sulla pipe*/
                    nw = write(piped[i/2][1],&ch,sizeof(ch));
                    if( nw != sizeof(ch)) {
                        printf("Errore: scrittura di un numero di byte sbagliati %d\n",nw);
                        exit(-1);
                    }
                }
                exit(k);
            }
            
        }
    }

    /*processo padre*/
    /*chiudiamo pipe che non utilizziamo*/
    for (k=0;k<N/2;k++) {
        close(piped[k][0]);
        close(piped[k][1]);
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
             printf("Il figlio con pid = %d ha ritornato %d (se 255 problemi!)\n",pidFiglio,ritorno);
         }
    }
    exit(0);

}
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
    char ch;
    int fd;	/*file descriptor per apertura file*/
    int nr,nw;	/*variabili per salvare/controllare valori di ritorno di read/write da/su pipe*/
    int pid;	/*pid per fork*/
    int pidFiglio,status,ritorno;	/*per valore di ritorno figli*/
    pipe_t *piped_1_2;
    pipe_t *piped_2_1;
    long int pos,posLetta;
    char Cx;
    int occ;
    /*----------------------------------------------*/

    /*controllo sul numero di parametri*/
    if (argc < 4) {
        printf("Errore numero di parametri, dato che argc=%d\n",argc);
        exit(1);
    }

    N = argc-2;

    /*Controllo che sia un singolo carattere*/
    if (strlen(argv[argc-1]) != 1) {
        printf("Errore: parametro passato %s non è un singolo carettere\n",argv[argc-1]);
        exit(2);
    }
    Cx= argv[argc-1][0];

    /*allocazione dell'array di N pipe descriptors*/
    piped_1_2= (pipe_t*)malloc(N*sizeof(pipe_t));
    piped_2_1= (pipe_t*)malloc(N*sizeof(pipe_t));
    if (piped_1_2 == NULL || piped_2_1 == NULL) {
        printf("Errore: problemi nell'allocazione di memoria per le pipe\n");
        exit(3);
    }

    for(j=0;j<N;j++) {
        if(pipe(piped_1_2[j]) < 0 || pipe(piped_2_1[j]) < 0) {
            printf("Errore: problemi nella creazione delle pipe\n");
            exit(4);
        }
    }

    printf("DEBUG-Sono il processo padre con pid %d e sto per creare %d figli \n",getpid(),N*2);
    for(i=0;i<N*2;i++) {
    
        if((pid = fork()) < 0) {
            printf("Errore nella fork\n");
            exit(5);
        }
        if(pid == 0) {
            /*codice figlio*/
            
            if (i < N) {
                /*codice del primo processo della coppia*/
                /*chiudiamo pipe che non utilizziamo*/
                for (k=0;k<N;k++) {
                    close(piped_1_2[k][0]);
                    close(piped_2_1[k][1]);
                    if(i != k) {
                        close(piped_1_2[k][1]);
                        close(piped_2_1[k][0]);
                    }
                }

                /*apro il file*/
                if ((fd = open(argv[i+1],O_RDONLY))<0) {
                     printf("Errore: impossibile aprire il file %s\n",argv[i+1]);
                    exit(-1);
                }

                occ = 0;
                while(read(fd,&ch,1)) {

                    if (ch == Cx) {

                        occ++;

                        /*abbiamo trovato l'occorrenza del carattere*/
                        pos = lseek(fd,0L,SEEK_CUR)-1;

                        /*scriviamo sulla pipe*/
                        nw = write(piped_1_2[i][1],&pos,sizeof(pos));
                        if( nw != sizeof(pos)) {
                            printf("Errore: scrittura di un numero di byte sbagliati %d\n",nw);
                            exit(-1);
                        }

                        /*leggiamo dalla pipe*/
                        nr = read(piped_2_1[i][0],&posLetta,sizeof(posLetta));
                        if( nr != sizeof(posLetta)) {
                            break; /*il processo 2 non ha trovato alcuna occorrenza*/
                        }

                        /*posizioniamoci nella posizione indicata dal secondo processo della coppia*/
                        lseek(fd,posLetta+1,SEEK_SET);

                    }
                }

                exit(occ);
            } else {

                /*codice del secondo processo della coppia*/

                /*chiudiamo pipe che non utilizziamo*/
                for (k=0;k<N;k++) {
                    close(piped_1_2[k][1]);
                    close(piped_2_1[k][0]);
                    if((2*N-i-1) != k) {
                        close(piped_1_2[k][0]);
                        close(piped_2_1[k][1]);
                    }
                }

                /*apro il file*/
                if ((fd = open(argv[2*N-i],O_RDONLY))<0) {
                     printf("Errore: impossibile aprire il file %s\n",argv[2*N-i]);
                    exit(-1);
                }

                /*leggiamo dalla pipe*/
                nr = read(piped_1_2[2*N-i-1][0],&posLetta,sizeof(posLetta));
                if( nr != sizeof(posLetta)) {
                    printf("Errore: lettura di un numero di byte sbagliati %d\n",nr);
                    exit(-1);
                }

                /*posizioniamoci nella posizione indicata dal primo processo della coppia*/
                lseek(fd,posLetta+1,SEEK_SET);
                occ = 0;
                while(read(fd,&ch,1)) {

                    if (ch == Cx) {
                        
                        occ++;

                        /*abbiamo trovato la posizione del carattere*/
                        pos = lseek(fd,0L,SEEK_CUR)-1;

                        /*scriviamo sulla pipe*/
                        nw = write(piped_2_1[2*N-i-1][1],&pos,sizeof(pos));
                        if( nw != sizeof(pos)) {
                            printf("Errore: scrittura di un numero di byte sbagliati %d\n",nw);
                            exit(-1);
                        }

                        /*leggiamo dalla pipe*/
                        nr = read(piped_1_2[2*N-i-1][0],&posLetta,sizeof(posLetta));
                        if( nr != sizeof(posLetta)) {
                            break; /*il processo 1 non ha trovato alcuna occorrenza*/
                        }

                        /*posizioniamoci nella posizione indicata dal primo processo della coppia*/
                        lseek(fd,posLetta+1,SEEK_SET);
                    }
                    
                }

                exit(occ);
            }
        }
    }

    /*chiudiamo le pipe che non utilizziamo*/
    for (j=0;j<N;j++) {
        close(piped_1_2[j][1]);
        close(piped_1_2[j][0]);
        close(piped_2_1[j][1]);
        close(piped_2_1[j][0]);
    }

    /*Il padre aspetta tutti i processi figli*/
    for(j=0;j<N*2;j++) {
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
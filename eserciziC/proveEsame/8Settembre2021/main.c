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
    int i,j,k,n;	/*indici*/
    char ch;
    int fd;	/*file descriptor per apertura file*/
    int nw;	/*variabili per salvare/controllare valori di ritorno di read/write da/su pipe*/
    int nr1,nr2;
    int *pid;	/*pid per fork*/
    int pidFiglio,status,ritorno;	/*per valore di ritorno figli*/
    int fcreato; /*file descriptor del file creato dal padre*/
    pipe_t *piped_1_P;
    pipe_t *piped_2_P;
    long int occ,maxocc;
    int linea;
    /*----------------------------------------------*/

    /*controllo sul numero di parametri*/
    if (argc < 2) {
        printf("Errore numero di parametri, dato che argc=%d\n",argc);
        exit(1);
    }   

    N =argc-1;

   /*creo un nuovo file*/
   if ((fcreato = creat("dimaio.log",0644)) < 0) {
        printf("Errore nella creazione del file\n");
        exit(2);
   }

   /*allocazione dell'array di N pipe descriptors*/
   piped_1_P= (pipe_t*)malloc(N*sizeof(pipe_t));
   piped_2_P= (pipe_t*)malloc(N*sizeof(pipe_t));
   if (piped_1_P == NULL || piped_2_P == NULL) {
        printf("Errore: problemi nell'allocazione di memoria per le pipe\n");
        exit(3);
   }

   for(j=0;j<N;j++) {
        if(pipe(piped_1_P[j]) < 0 || pipe(piped_2_P[j]) < 0) {
            printf("Errore: problemi nella creazione delle pipe\n");
            exit(4);
        }
   }

    /*alloco l'array di pid*/
   pid = (int*)malloc(N*2*sizeof(int));
   if (pid == NULL) {
    printf("Errore nella allocazione\n");
    exit(5);
   }

   printf("DEBUG-Sono il processo padre con pid %d e sto per creare %d figli \n",getpid(),N*2);
   for(n=0;n<N*2;n++) {
   
    if((pid[n] = fork()) < 0) {
        printf("Errore nella fork\n");
        exit(6);
    }
    if(pid[n] == 0) {
        /*codice figlio*/

       
        
        if (n % 2 == 0) {
            /*processi pari*/

             /*chiudiamo pipe che non utilizziamo*/
            for (k=0;k<N;k++) {
                close(piped_1_P[k][0]);
                close(piped_2_P[k][0]);
                close(piped_2_P[k][1]);
                if(n/2 != k) close(piped_1_P[k][1]);
            }
            
            /*apro il file*/
            if ((fd = open(argv[(n/2)+1],O_RDONLY))<0) {
                 printf("Errore: impossibile aprire il file %s\n",argv[(n/2)+1]);
                exit(-1);
            }

            occ = 0; /*numero di caratteri per riga*/
            maxocc=0;
            linea = 1;
            while(read(fd,&ch,1)) {
                
                if (ch == '\n') {
                    
                    occ++;
                    
                    if (linea % 2 == 0) {
                        /*scriviamo sulla pipe*/
                        nw = write(piped_1_P[n/2][1],&occ,sizeof(occ));
                        if( nw != sizeof(occ)) {
                            printf("Errore: scrittura di un numero di byte sbagliati %d\n",nw);
                            exit(-1);
                        }  

                        if (occ > maxocc) maxocc = occ;
                        
                    } 
                    /*incrementiamo comunque il numero di linee*/
                    linea++;
                    occ = 0;

                } else {
                    occ++;
                }

            }
            exit(maxocc);
        } else {
            /*processi dispari*/

            /*chiudiamo pipe che non utilizziamo*/
            for (k=0;k<N;k++) {
                close(piped_2_P[k][0]);
                close(piped_1_P[k][0]);
                close(piped_1_P[k][1]);
                if(n/2 != k) close(piped_2_P[k][1]);
            }

            /*apro il file*/
            if ((fd = open(argv[(n/2)+1],O_RDONLY))<0) {
                 printf("Errore: impossibile aprire il file %s\n",argv[(n/2)+1]);
                exit(-1);
            }

            occ = 0; /*numero di caratteri per riga*/
            maxocc=0;
            linea = 1;
            while(read(fd,&ch,1)) {
                if (ch == '\n') {
                    
                    occ++;
                    
                    if (linea % 2 == 1) {
                        /*scriviamo sulla pipe*/
                        nw = write(piped_2_P[n/2][1],&occ,sizeof(occ));
                        if( nw != sizeof(occ)) {
                            printf("Errore: scrittura di un numero di byte sbagliati %d\n",nw);
                            exit(-1);
                        }  

                        if (occ > maxocc) maxocc = occ;
                        
                    } 
                    /*incrementiamo comunque il numero di linee*/
                    linea++;
                    occ = 0;

                } else {
                    occ++;
                }

            }
            exit(maxocc);
        }
    
    
    }
   
   }

   /*codide padre*/
   /*chiudiamo pipe che non utilizziamo*/
   for (k=0;k<N;k++) {
    close(piped_1_P[k][1]);
    close(piped_2_P[k][1]);
    }

    long int L1;
    long int L2;
    for (i=0;i<N;i++) {
        linea = 1;

        printf("Le lunghezze delle linee del file %s sono:\n",argv[i+1]);
        while(1) {
            nr1 = read(piped_2_P[i][0],&L2,sizeof(L1)); /*processi dispari*/

            nr2 = read(piped_1_P[i][0],&L1,sizeof(L2)); /*processi pari*/
            
            if (nr2 != sizeof(L2)) {
                printf("la linea numero %d è lunga %ld\n",linea,L2);
                linea++;
            }
            
            if (nr1 != sizeof(L1)) {
                printf("la linea numero %d è lunga %ld\n",linea,L1);
                linea++;
            }
           

           
            

            if ((nr1 + nr2) == 0) {
                break;
            }

        }
    }

    /*Il padre aspetta tutti i processi figli*/
    for(j=0;j<N*2;j++) {
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
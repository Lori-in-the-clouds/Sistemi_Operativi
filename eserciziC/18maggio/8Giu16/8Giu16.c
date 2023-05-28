#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>

int mia_random(int n) {
    int casuale;
    casuale = rand() % n;
    return casuale;
}

typedef int pipe_t[2];

int main(int argc,char **argv) {

pipe_t *piped_FP;
pipe_t *piped_PF;
int j,k;
int N,H;
int status,ritorno;
int pid;
int fd,fdout;
int index;
char linea[255];
int modified;
int random;
int good;


/*controllo sul numero di parametri*/
if (argc < 6) {
    printf("Errore numero di parametri, dato che argc=%d\n",argc);
    exit(1);
}

N = argc -2;
H = atoi(argv[argc-1]);

/*controllo sull'ultimo parametro*/
if (H >= 255  || H <= 0) {
    printf("Errore sull'ultimo parametro passato,deve essere strettmanete maggiore di 0 e minore d 255");
    exit(2);
}

/*inizializziamo seme per generare numeri random*/
srand(time(NULL));

/*creiamo un file temporaneo*/
if ((fdout= open("/tmp/creato", O_CREAT|O_WRONLY|O_TRUNC,0644)) < 0) {
    printf("Errore nella creazione del file %s\n","tmp/creato");
    exit(3);
}

/*allocazione dell'array di N pipe descriptors*/
if ((piped_FP = (pipe_t*)malloc(N*sizeof(pipe_t))) == NULL || (piped_PF = (pipe_t*)malloc(N*sizeof(pipe_t))) == NULL ) {
    printf("Errore nella creazione della pipe\n");
    exit(4);
}

/*creazione delle N pipe figli-padre*/   
for (j=0;j<N;j++) {
        if (pipe(piped_FP[j]) < 0 || pipe(piped_PF[j]) < 0) {
            printf("Errore nella creazione della pipe\n");
            exit(5);
        }
}


/*generazione figli*/
for (j=0;j<N;j++) {
    
    if ((pid = fork()) < 0) {
        printf("Errore nella fork\n");
        exit(6);
    }

    if (pid == 0) {
        /*processo figlio*/
        
        /*chiudiamo le pipe che non utilizziamo*/
        for (k=0;k<N;k++) {
            close(piped_FP[k][0]);
            close(piped_PF[k][1]);

            if (k != j) {
                close(piped_FP[k][1]);
                close(piped_PF[k][0]);
           }
        }

        /*apriamo il file*/
        if ((fd = open(argv[j+1],O_RDONLY)) < 0) {
            printf("Errore nella apertura del file\n");                
            exit(-1);
        }

        k = 0; /*conta le occorenze*/
        int riga = 0;
        while(read(fd,&linea[k],sizeof(linea[k]))) {
            
            if (linea[k] == '\n') {
                
                /*contiamo anche il terminatore di linea*/
                k++;

                /*inviamo al padre la lunghezza della linea*/
                write(piped_FP[j][1],&k,sizeof(k));

                /*leggiamo l'indice inviato dal padre*/
                read(piped_PF[j][0],&index,sizeof(index));

                if (index < k) {
                    /*possiamo prelevare tale valore dalla riga e scriverlo nel file*/
                    printf("Era stato generato l'indice %d per la riga %d, valore %c viene scritto nel file\n",index,riga,linea[index]);
                    write(fdout,&linea[index],sizeof(linea[index]));
                    modified++;
                } else {
                    printf("Era stato generato l'indice %d per la riga %d, non può essere utilizzato\n",index,riga);
                }

                /*abbiamo finito,leggiamo la prossima riga*/
                k = 0;
                riga++;
                continue;
            }
            k++;
        }
        exit(modified);
    }
}

/*codide padre*/

/*chiudiamo i lati delle pipe che non utilizziamo*/
for (j=0;j<N;j++) {
    close(piped_FP[j][1]);
    close(piped_PF[j][0]);
}

for (j=1;j<=H;j++) {

    /*calcoliamo il valore random*/
    random = mia_random(N);

    for (j=0;j<N;j++) {
        /*il padre recupera tutti i valori dei figli*/
        read(piped_FP[j][0],&index,sizeof(index));

        /*ma salva solo quello che è indicato dal numero random*/
        if (j == random) {
            good = index;
        }
    }

     /*il padre calcola in modo random l'indice da inviare a tutti i figli*/
     for (k=0;k<N;k++) {
        random = mia_random(good);
        write(piped_PF[k][1],&random,sizeof(random));
     }
}

/*il padre aspetta il figlio*/
for (j=0;j<N;j++) {
        if ((pid = wait(&status)) < 0) {
            printf("Errore nella wait\n");
            exit(5);
        }

        if ((status & 0xFF) != 0) {
            printf("Figlio con pid %d terminato in modo anomalo\n",pid);
        }
        else {
            ritorno = (int)((status >> 8) & 0xFF);
                printf("Il figlio con pid=%d ha ritornato %d(se 255 errore)\n",pid,ritorno);        
        }

    }
    exit(0);


}
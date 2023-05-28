#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc,char **argv) {
    int i;
    int nr;
    char *buffer;
    int fd;
    //controllo il numero di parametri
    if (argc!=3) {
        printf("Errore: numero di parametri passato errato");
        exit(1);
    }
    //controlliamo che il primo parametro sia un nume di un file
    if ((fd = open(argv[1],O_RDONLY)) == -1) {
        printf("ERRORE: primo parametro passato non è un nome di un file");
        exit(2);
    }

  
    if (atoi(argv[2]) < 0) {
        printf("Errore: secondo parametro passato non è un numero strettamente positivo");            
        exit(3);
    }
   

    //iniziamo a leggere i caratteri
    int N = atoi(argv[2]);
    buffer = (char *)(malloc(N* sizeof(char)));
    //controlliamo il valore di ritorno della funzione malloc
    if (buffer == NULL) {
        printf("Errore: problemi con la malloc\n");
        exit(5);
    }
    //stampiamo tutti i caratteri del file F che si trovano in posizione multipla di N 
    printf("stampiamo tutti i caratteri del file F che si trovano in posizione multipla di %d\n",N);
    i = 1;
    while((nr = read(fd,buffer,N)) > 0) {
        if (nr == N) {
            printf("Il carattere %d-esmimo all'intenrno del file %s è %c\n",i,argv[1],buffer[N-1]);
            //incrementiamo i
            i++;
        }
    }

    exit(0);
}
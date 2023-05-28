#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc,char **argv) {
    int endpos;
    int fd;
    int n;
    char buffer;
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

  
    if ((n = atoi(argv[2])) < 0) {
        printf("Errore: secondo parametro passato non è un numero strettamente positivo");            
        exit(3);
    }

    endpos = lseek(fd,0L,SEEK_END);
    endpos--;
    //stampiamo il numero di caratteri del file
    printf("Nuemro di caratteri del file: %d\n",endpos);
    
    int j=1;
    for(int i=0;i< endpos;i+=n) {
        if (i==0) i=1;
        lseek(fd,i,SEEK_SET);
        read(fd,&buffer,1);
        printf("Carattere %d-esimo: %c\n",j,buffer);
        j++;
        if (i==1) i--;
    }
}
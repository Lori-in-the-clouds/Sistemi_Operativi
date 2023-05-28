#include <stdio.h>
#include <stdlib.h>
int main(int argc, char **argv) {
    int N = argc;
    //controllo che il numero di parametri sia corretto
    if (argc < 2) {
        printf("Errore,numero di parametri passato errato");
        //nel caso di errore 
        exit(1);
    }
    //stampiamo il nome dell'eseguibile
    printf("Nome dell'eseguibile: %s\n",argv[0]);
    //stampiamo il numero di parametri
    printf("Numero di parametri passsati: %d\n",N-1);
    for(int i=0;i<N-1;i++) {
        printf("Valore del parametro %d: %s\n",i,argv[i+1]);
    }
    exit(0);
}
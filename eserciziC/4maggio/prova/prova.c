#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <fcntl.h> 
#include <sys/wait.h>

int main(int argc, char **argv) {
    char dom;
    printf("Si vuole eseguire la chiamata ricorsiva?");
    scanf("%s",&dom);
    if (dom == 's' || dom == 'S') {
        printf("Eseguo chiamata ricorsiva:\n");
        //execl("prova","prova",(char *)0); //funziona anche con nome assoluto
        execlp("./prova","prova",(char*) 0);
        printf("Errore nella chiamata ricorsiva\n");
        exit(2);
    } else {
        exit(0);
    }

}
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>

int main (int argc, char** argv)
{
    int fd;
    int pid;
    int pidFiglio, status, ritorno;         /* per wait padre */

    //controllo sul numero di parametri
    if (argc != 2) {
        printf("Errore sul numero di parametri");
        exit(1);
    }

   

    if ((pid = fork()) < 0) {
        printf("Errore nella fork");
        exit(3);
    }

    //processo figlio
    if (pid == 0) {
        //ridirezine standard input
        close(0);
        
        //controllo che il primo parametro sia un nome di un file
        if ((fd = open(argv[1],O_RDONLY)) < 0) {
        printf("Errore, il parametro passato non è un file");
        exit(2);
        }
        
        execl("/home/lorenzo/SOeLab22-23/Lezioni/C/mycat","mycat",argv[1],(char *)0);
        perror("Errore nella exec\n");
        exit(errno);
    }

    if ((pidFiglio = wait(&status)) < 0) {
        printf("Errore nella wait\n");
        exit(5);
    }
    if ((status & 0xFF) != 0)
        	printf("Figlio con pid %d terminato in modo anomalo\n", pidFiglio);
	else {
        ritorno = (int)((status >> 8) & 0xFF);
        printf("Il figlio con pid %d ha ritornato il valore: %d\n",pidFiglio,ritorno);
    }

    exit(0);





}
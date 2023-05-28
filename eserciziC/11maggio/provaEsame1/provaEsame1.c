#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <ctype.h>


int main(int argc, char **argv) {
   int p[2][2];
   int N = argc -1;
   int pid;
   int status,ritorno;
   int j;
   int fd;
   char ch,ch_num,ch_var;
   int cont1,cont2;
   int sum;

   /*controllo sul numero di parametri*/
   if (argc < 3) {
    printf("Errore: numero di parametri passato errato\n");
    exit(1);
   }

   /*apriamo le due pipe*/
   
   /*pipe 0 = comunicazione processi pari*/
   if (pipe(p[0]) < 0) {
        printf("Errore nella pipe\n");
        exit(2);
   }
    /*pipe 1 = comunicazione processi dispari*/
   if (pipe(p[1]) < 0) {
        printf("Errore nella pipe\n");
        exit(3);
   }

   for(j=0;j<N;j++) {
        
        if ((pid = fork()) < 0) {
                printf("Errore nella fork\n");
                exit(4);
        }

        if (pid == 0) {
            //codice del figlio

            /*apertura del file*/
            if ((fd = open(argv[j+1],O_RDONLY)) < 0) {
                printf("Errore nella open del file %s",argv[j+1]);
                exit(5);
            }

            /*chiudiamo la lettura delle due pipe*/
            close(p[0][0]);
            close(p[1][0]);

            
            while(read(fd,&ch,1) > 0) {
                if ( ((j % 2 == 0) && (isdigit(ch)))) {
                    write(p[0][1],&ch,1);
                }
                if ( ((j % 2 == 1) && (isalpha(ch)))) {
                    write(p[1][1],&ch,1);
                }
                    //printf("DEBUG: %c\n",ch);

            }
            exit(ch);
        }
    }
     
     /*il padre riceve i messaggi della pipe*/
        /*chiudiamo la scrittura delle pipe*/
        close(p[0][1]);
        close(p[1][1]);
        
        cont1 = read(p[0][0],&ch_num,1);
        cont2 = read(p[1][0],&ch_var,1);

        while((cont1 > 0) && (cont2 > 0)) {
            sum += (cont1+cont2);
            printf("%c\n",ch_num);
            printf("%c\n",ch_var);
            cont1 = read(p[0][0],&ch_num,1);
            cont2 = read(p[1][0],&ch_var,1);
        }
       
        while((cont1 = read(p[0][0],&ch_num,1)) > 0) {
            printf("%c\n",ch_num);
            sum += cont1;
        }
        while((cont2 = read(p[1][0],&ch_var,1)) > 0) {
             printf("%c\n",ch_var);
             sum += cont2;
        }
        

        printf("\nNumero totale di caratteri ricevuti: %d\n", sum);


        /*il padre asptetta il figlio*/
        for(j=0;j<N;j++) {
            
            if ((pid = wait(&status)) < 0) {
                printf("Errore nella wait\n");
                exit(6);
            }

            if ((status & 0xFF) != 0) {
                printf("Il figlio con pid %d terminato in modo anomalo\n",pid);
            }
            else {
                ritorno= ((status >> 8) & 0xFF);
                printf("Il figlio con pid %d ha ritornato %c\n",pid,ritorno);
            }
        }

        exit(0);
            

}




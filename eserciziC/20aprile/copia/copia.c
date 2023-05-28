#include <stdio.h> //inserito per utilizzare printf e altre funzioni di I/O
#include <stdlib.h> // inserito per poter utilizzare la primitiva exit e malloc ecc..
#include <unistd.h> //inserito per poter utilizzare la primitiva close
#include <fcntl.h> //inserito per poter utilizzare le costanti per la open
#define PERM 0644   /* in ottale per diritti UNIX */

int copyfile (char *f1, char * f2)
{    int infile, outfile, nread;	/* usati per open e valore ritorno read */
     char buffer[BUFSIZ];  		/* usato per i caratteri */

	if ((infile = open(f1, O_RDONLY)) < 0) return 2;
	/* ERRORE se non si riesce ad aprire in LETTURA il primo file */

	if ((outfile = creat(f2, PERM)) <0 )
	/* ERRORE se non si riesce a creare il secondo file */
        {	close(infile); 
		return 3; 
	}

	while ((nread = read(infile, buffer, BUFSIZ)) > 0 )	/* ciclo di lettura fino a che nread e' maggiore di 0 */
	{ 
		if (write(outfile, buffer, nread) < nread)	/* scriviamo sempre nread byte! */
		/* ERRORE se non si riesce a SCRIVERE */
    		{ 	close(infile); 
			close(outfile); 
			return 4; 
		}
	}	
	/* se arriviamo qui, vuol dire che tutto e' andato bene */
	close(infile); 
	close(outfile); 
	return 0;
}

int main (int argc, char** argv)
{   int status;
	if (argc != 3)  /* controllo sul numero di parametri: devono essere esattamente 2! */
     	{ 	printf("Errore: numero di argomenti sbagliato dato che argc = %d\n", argc);
       		printf("Ci vogliono 2 argomenti: nome-file-sorgente e nome-file-destinazione\n");
       		exit(1); 
	}
	status = copyfile(argv[1], argv[2]);	/* invochiamo la funzione copyfile passando le due stringhe passate come parametri */
	if (status != 0) 
 		printf("Ci sono stati degli errori durante la copia dato che status = %d\n", status);

	exit(status);
}
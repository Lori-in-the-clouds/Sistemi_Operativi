/* versione con N pipe di comunicazione/sincronizzaione fra padre e figli e altre N di comunicazione fra figli e padre; inoltre uso del segnale SIGKILL */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h> 
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>

#define PERM 0644

typedef int pipe_t[2];

int main (int argc, char **argv)
{
   int N; 				/* numero di file */
   int *pid;				/* array di pid per fork: N.B. in questo caso serve un array di pid perche' il padre deve inviare il segnale SIGKILL per terminare forzatamente i figli non ancora terminati */
   pipe_t *pipeFiglioPadre;		/* array di pipe di comunicazione fra figli e padre */
   pipe_t *pipePadreFiglio;		/* array di pipe di comunicazione/sincronizzazione fra padre e figlio. NOTA BENE: questa sincronizzazione potrebbe essere fatta tramite l'invio di segnali da parte del padre ai figli */
   int i,j; 				/* contatori */
   int fdw,fd; 				/* file descriptor */
   int pidFiglio, status, ritorno;	/* per valore di ritorno figli */
   char c; 				/* carattere per leggere dal file da parte dei figli e dalle pipe da parte del padre */
   int primoindice; 			/* serve per salvare l'indice del primo figlio che ha finito di leggere */
   char token='x'; 			/* carattere il cui valore non importa e che serve solo per sincronizzare i figli */
   int nr; 				/* numero di byte letti da file o da pipe */

	/* ci vogliono almeno due file */
	if (argc < 3)
	{	
		printf("Errore numero di parametri, dato che argc=%d\n", argc);
		exit(1);
	}

	N = argc-1; 	/* calcoliamo il numero di file e quindi di processi da creare */
	printf("DEBUG-Numero di processi da creare %d\n", N);

	/* allocazione pid */
	if ((pid=(int *)malloc(N*sizeof(int))) == NULL)
	{
        	printf("Errore allocazione pid\n");
        	exit(2);
	}

	/* allocazione pipe figli-padre */
	if ((pipeFiglioPadre=(pipe_t *)malloc(N*sizeof(pipe_t))) == NULL)
	{
		printf("Errore allocazione pipe padre\n");
		exit(3);
	}

	/* allocazione pipe padre-figli */
	if ((pipePadreFiglio=(pipe_t *)malloc(N*sizeof(pipe_t))) == NULL)
	{
		printf("Errore allocazione pipe padre\n");
		exit(4);
	}

	/* creazione pipe */
	for (i=0;i<N;i++)
		if(pipe(pipeFiglioPadre[i])<0)
		{
			printf("Errore creazione pipe\n");
			exit(5);
		}

	/* creazione di altre N pipe di comunicazione/sincronizzazione con il padre */
	for (i=0;i<N;i++)
		if(pipe(pipePadreFiglio[i])<0)
		{
			printf("Errore creazione pipe\n");
			exit(6);
		}

	/* creazione file usato solo dal padre: N.B. il testo dice "Il processo padre deve, per prima cosa creare un file di nome "Merge" e, quindi, deve generare N processi figli ..." QUINDI LA CREAZIONE VA FATTA PRIMA DI CREARE I FIGLI! */
	if ((fdw=creat("Merge",PERM))<0)
        {
        	printf("Impossibile creare il file Merge\n"); 
        	exit(7);
        }

	/* creazione figli con salvataggio dei pid nell'array */
	for (i=0;i<N;i++)
	{
		if ((pid[i]=fork())<0)
		{
			printf("Errore creazione figli\n");
			exit(8);
		}	
		if (pid[i]==0)
		{ 	/* codice figlio */
			printf("DEBUG-Sono il figlio %d di indice %d e sono associato al file %s\n", getpid(), i, argv[i+1]);
			/* chiusura pipe inutilizzate */
			for (j=0;j<N;j++)
			{
			        /* il figlio non legge dai nessuna pipeFiglioPadre e non scrive su nessuna pipePadreFiglio */
				close (pipeFiglioPadre[j][0]);
				close (pipePadreFiglio[j][1]);
				if (j != i) 
 				{	/* inoltre non scrive e non legge se non su/dalle sue pipe */
					close (pipeFiglioPadre[j][1]);
					close (pipePadreFiglio[j][0]);
	 			}	
			}	
			/* apertura file */
			if ((fd=open(argv[i+1],O_RDONLY))<0)
			{
				printf("Impossibile aprire il file %s\n", argv[i+1]);
				exit(-1); /* torniamo -1 che e' un valore che non puo' essere interpretato come carattere */
			}
			/* ciclo di lettura da file solo dopo aver ricevuto l'indicazione dal padre */
        		do
        		{
				/* per prima cosa si attende l'indicazione che si puo' procedere inviata dal padre */
                		read(pipePadreFiglio[i][0], &token, 1);
                		nr=read(fd,&c,1);
                		/* printf("HO LETTO IL TOKEN per il carattere %c\n", c); */
                		/* se il figlio e' riuscito a leggere un carattere dal file lo manda al padre */
                		if (nr)
                		{
                			write(pipeFiglioPadre[i][1],&c,1);
                		}
        		} while (nr);	/* questo ciclo continua fino a che nr e' uguale a 1 e cioe' il figlio legge caratteri dal file */

         		/* il file e' terminato e quindi si deve ritornare al padre il carattere richiesto */
        		exit(c);
		}
	} /* fine for */

	/* codice del padre */
	/* chiusura pipe */
	for(i=0;i<N;i++)
	{	/* il padre non legge da nessuna pipePadreFiglio e non scrive su nessuna pipeFiglioPadre */
		close (pipePadreFiglio[i][0]);
		close (pipeFiglioPadre[i][1]);
	}

	/* il padre deve leggere i caratteri inviati dai figli fino a che ce ne sono */
	nr=1;
	while (nr)
		for(i=0;i<N;i++)
		{
			/* il padre manda l'indicazione di leggere ad ogni figlio per ogni carattere */
			write(pipePadreFiglio[i][1], &token, 1);
			/* quindi il padre si mette in attesa del carattere corrente letto dal figlio i-esimo */
			nr=read(pipeFiglioPadre[i][0],&c,1);
			if (nr)
			{
				/* printf("Figlio di indice %d mi ha mandato il carattere %c dal file %s\n", i, c, argv[i+1]); */
				/* se il padre riceve un carattere dal figlio i-esimo, lo deve scrivere sul file "Merge" */
				write(fdw, &c, 1);
			}
			else 
			{
				/* printf("Figlio di indice %d non ha inviato nulla\n", i); */
				/* se il padre NON riceve un carattere dal figlio i-esimo, allora vuole dire che il figlio e' terminato quindi la pipe i-esima risulta una pipe SENZA scrittore e quindi al padre torna il valore 0 e quindi il padre riesce a capire che il figlio i-esimo e' terminato e può salvare l'indice nella variabile primoindice */
				primoindice=i;
				break; /* appena un figlio non invia nulla si deve uscire dal ciclo for e poi, di conseguenza, dato che nr è 0 si esce anche dal ciclo while */
			}
		}	
	printf("DEBUG-Valore di primoindice %d\n", primoindice);
	/* il padre deve terminare forzatamente gli altri figli (che sono in attesa della indicazione del padre di procedere) a parte quello gia' terminato, mandando il segnale SIGKILL che non puo' essere ignorato o intercettato */
	for(i=0;i<N;i++)
 		if (i != primoindice) 
			if (kill(pid[i], SIGKILL) == -1) /* controlliamo che la kill non fallisca a causa della terminazione di uno dei figli */
				printf("Figlio con pid %d non esiste e quindi e' gia' terminato\n", pid[i]);

	/* Il padre aspetta i figli */
	for (i=0; i < N; i++)
	{
		if ((pidFiglio = wait(&status)) < 0)
        	{
                	printf("Errore in wait\n");
                	exit(9);
        	}

		if ((status & 0xFF) != 0)
                	printf("Figlio con pid %d terminato in modo anomalo\n", pidFiglio);
        	else
        	{	 
			ritorno=(int)((status >> 8) & 0xFF);
			if (ritorno == 255) 
				printf("Il figlio con pid=%d ha avuto un errore\n", pidFiglio);
			else
                		printf("Il figlio con pid=%d ha ritornato il carattere %c\n", pidFiglio, ritorno);
		}
	}

	exit(0);
}

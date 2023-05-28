printf("DEBUG-Sono il processo padre con pid %d e sto per creare %d figli \n",getpid(),);
for(i=0;i<N;i++) {

    if((pid = fork()) < 0) {
        printf("Errore nella fork\n");
        exit();
    }
    if(pid == 0) {
        /*codice figlio*/
        //chiudiamo pipe che non utilizziamo
    }
}

/*Il padre aspetta tutti i processi figli*/
for(=0;<;++) {
    if((pidFiglio = wait(&status)) < 0) {
        printf("Errore wait\n");
        exit();
    }
    if((status & 0xFF) != 0) {
        printf("Figlio con pid %d terminato in modo anomalo\n",pidFiglio);
         exit();
    } else {
         ritorno = (int)((status >> 8) & 0xFF);
         printf("Il figlio con pid = %d ha ritornato %d (se 255 problemi!)\n",pidFiglio,ritorno);
     }
}
exit(0);

if ((fd = open(argv[],O_w))<0) {
     printf("Errore: impossibile aprire in  il file %s\n",argv[O_RDONLY]);
    exit(-1);
}

/*chiudiamo pipe che non utilizziamo*/
/*ATTENZIONE DA CANCELLARE: q oppure i*/
for(j=0;j<;j++) {
    if(j != q)
        close(piped[j][0]);
    if(j != (q+1)%)
        close(piped[j][1];
}

/*chiudiamo pipe che non utilizziamo*/
/*ATTENZIONE DA CANCELLARE: q oppure i*/
for(j=0;j<Q;j++) {
    if(j != q)
        close(piped[j][0]);
    if(j != (q+1)%Q)
        close(piped[j][1]);
}

/*chiudiamo le pipe che non utilizziamo*/
for (j=0;j<N;j++) {
    close(piped[j][1]);
}

/*chiudiamo le pipe che non utilizziamo*/
for (j=0;j<N;j++) {
    close(piped_FP[j][1]);
    close(piped_PF[j][0]);
}

/*chiudiamo le pipe che non utilizziamo, apparte la prima perchè il padre deve dare il primo ok al primo figlio*/
/*si lascia aperto sia il lato di scrittura che viene usata (e poi in effetti chiuso) che il lato di lettura*/
for (j=1;j<Q;j++) {
    close(piped[j][1]);
    close(piped[j][0]);
}

/*chiudiamo le pipe che non utilizziamo*/
for (j=0;j<N;j++) {
    close(piped_FP[j][1]);
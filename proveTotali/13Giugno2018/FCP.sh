#!/bin/sh

#controllo il numero di parametri, controllo (stretto/lasco)
case $# in
	

	0|1|2|3) #errore, numero di parametri non corretto
		echo Errore: numero di parametri non corretto
		exit 1;;
	*);; #vuoto
esac

 #controlliamo che il primo parametro sia il nome relativo semplice di una directory
 case $1 in
 	*/*)
 		echo Errore: $1 non in forma relativa semplice; exit 2;;
 	*);;
 		#ok
 esac
 
 D=$1 #salviamo nella variabile $D il valore del primo parametro
 
 #controllo che il parametro $2 sia un numero strettamente positivo
 case $2 in
 	*[!0-9]*)
 		echo Errore: parametro $2 non numerico o non positivo
 		exit 3;;	
 	0)
 		echo Errore: parametro $2 uguale a 0
 		exit 4;;
 esac
 
 Y=$2	#salviamo nella variabile $Y il valore del secondo parametro
 
 #eseguiamo una doppia shift
 shift 
 shift
 
 #controlliamo i restanti parametri
 for i 
 do
	 #controlliamo che sia il nome assoluto di una gerarchia e quindi una directory attraversabile(non richiesta dalla consegna ma necessaria)
	 case $i in
	 	/*)
	 		if test ! -d $i -o ! -w $i	#se nome assoluto, controlliamo sia una dir attraversabile
	 			then 
	 				echo Errore: $i non directory non attraversabile
	 				exit 5
	 		fi;;
	 	*)
	 		echo Errore: $1 non nome assoluto; exit 6;;
	 esac
done
 
 #se arriviamo qui abbiamo finito i controlli sui parametri
 #quindi possiamo passare a settare la variabile di ambiente PATH
 
 PATH=`pwd`:$PATH
 export PATH
 
 #eseguiame le W fasi
 for G
 do
 	#chiamata al file ricorsivo
 	FCR.sh $G $D $Y
 done
 
 
 
 

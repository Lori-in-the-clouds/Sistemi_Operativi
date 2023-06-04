#!/bin/sh

#controllo il numero di parametri, controllo lasco
case $# in
	0|1|2|3) #errore, numero di parametri non corretto
		echo Errore: numero di parametri non corretto
		exit 1;;
	*);; #vuoto
esac

 #controlliamo che sia il nome assoluto di una gerarchia e quindi una directory attraversabile (non richiesto esplicitamente dal testo ma necessaria)
 case $1 in
 	/*)
 		if test ! -d $1 -o ! -w $1	#se nome assoluto, controlliamo sia una dir attraversabile
 			then 
 				echo Errore: $1 non directory non attraversabile
 				exit 2
 		fi;;
 	*)
 		echo Errore: $1 non nome assoluto; exit 3;;
 esac
 
 #inseriamo nella variabile G il valore $1
 G=$1
 
 #controllo che il parametro $ sia un numero strettamente positivo
 case $2 in
 	*[!0-9]*)
 		echo Errore: parametro $ non numerico o non positivo
 		exit 4;;	
 	0)
 		echo Errore: parametro $ uguale a 0
 		exit 5;;
 esac
 
#inseriamo nella variabiel L il valore $2
L=$2

#siccome abbiamo salvato già i primi due parametri all'interno di variabili,eseguiamo la doppia shift
shift 
shift

#controlliamo che i restanti parametri siano singoli caratteri
for i
do
	 #controllo che il parametro $i sia un singolo carattere
	 case $i in
	 	?) ;; #ok è un singolo carattere
	 	*)
	 		echo Errore: $i non singolo carattere
	 		exit 6;;
 	esac
done

 #se arriviamo qui abbiamo finito i controlli sui parametri
 #quindi possiamo passare a settare la variabile di ambiente PATH
 
 PATH=`pwd`:$PATH
 export PATH
 
 #creo un file temporaneo
 > /tmp/nomiAssoluti
 
 #chiamata al file ricorsivo
 FCR.sh $G $L /tmp/nomiAssoluti $*
 
 #contiamo il numero di file trovati
 nl=`wc -l < /tmp/nomiAssoluti`
 
 #stampiamo il numero di file trovati
 echo Numero di file trovati: $nl
 
 #per ognuno dei file trovati invochiamo la parte C
 for i in `cat /tmp/nomiAssoluti`
 do
 	main $i $L $*
 done
 
 #cancelliamo il file temporaneo
 rm /tmp/nomiAssoluti
 
 
 
 
 

#!/bin/sh

#controllo il numero di parametri, controllo (stretto/lasco)
case $# in
	
	0|1|2|3) #errore, numero di parametri non corretto
		echo Errore: numero di parametri non corretto
		exit 1;;
	*) ;; #vuoto
esac

#controlliamo che sia il nome assoluto di una gerarchia e quindi una directory attraversabile
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


 
 #controllo che il parametro $2 sia un numero strettamente positivo
 case $2 in
 	*[!0-9]*) 
 		echo Errore: parametro $2 non numerico o non positivo
 		exit 4;;	
 	0)
 		echo Errore: parametro $2 uguale a 0
 		exit 5;;
 esac
 
 G=$1
 L=$2
 
 #eseguiamo una doppia shift, visto che abbiamo già salvato all'interno di parametri i primi due
 shift
 shift
 
 #controlliamo che i restanti parametri siano singoli caratteri
 for i 
 do
 	 #controllo che il parametro $i sia un singolo carattere
	 case $i in
	 	?) ;;
	 	*)
	 		echo Errore: $i non singolo carattere
	 		exit 6;;
	 esac
done

 #se arriviamo qui abbiamo finito i controlli sui parametri
 #quindi possiamo passare a settare la variabile di ambiente PATH
 
 PATH=`pwd`:$PATH
 export PATH
 
 #azzeriamo il file temporaneo
 > /tmp/nomiAssoluti
 
 #singola chiamata ricorsiva
 FCR.sh $G $L /tmp/nomiAssoluti $*
 
 echo Numero di file che soddisfa le specifiche: `wc -l < /tmp/nomiAssoluti`
 
 for i in `cat /tmp/nomiAssoluti`
 do
 	#invocazione parte C
 	#main $i $L $*
 	echo $i
 done
 
 #cancelliamo l'ultimo file temporaneo
 rm /tmp/nomiAssoluti
  






#!/bin/sh

#controllo il numero di parametri, controllo (stretto/lasco)
case $# in
	0|1|2|3) #errore, numero di parametri non corretto
		echo Errore: numero di parametri non corretto
		exit 1;;
	*);; #vuoto
esac

#controlliamo che il primo parametro sia un nome relativo semplice di una directory attraversabile
case $1 in
	
	/* | */*)
 		echo Errore: $1 non nome relativa semplice; exit 2;;
 		
 	*);;

 esac
 
 D=$1
 
 #controllo che il parametro $2 sia un numero strettamente positivo
 case $2 in
 	*[!0-9]*)
 		echo Errore: parametro $2 non numerico o non positivo
 		exit 4;;	
 	0)
 		echo Errore: parametro $2 uguale a 0
 		exit 5;;
 esac
 
 Y=$2
 
 #eseguiamo doppia shift
 shift 
 shift
 
 
 #controlliamo i restanti parametri
 for i
 do
	 #controlliamo che sia il nome assoluto di una gerarchia e quindi una directory attraversabile
	 case $i in
	 	/*)
	 		if test ! -d $i -o ! -w $i	#se nome assoluto, controlliamo sia una dir attraversabile
	 			then 
	 				echo Errore: $i non directory non attraversabile
	 				exit 6
	 		fi;;
	 	*)
	 		echo Errore: $i non nome assoluto; exit 7;;
	 esac
 done
 
 #se arriviamo qui abbiamo finito i controlli sui parametri
 #quindi possiamo passare a settare la variabile di ambiente PATH
 
 PATH=`pwd`:$PATH
 export PATH
 
 count=0
 
 #eseguiamo la chiamata al file ricorsivo
 for i 
 do
 	#creiamo un file temporaneo
 	> /tmp/creato-$count
 	
 	FCR.sh $i $D $Y /tmp/creato-$count
 	
 	#contiamo il numero di file che abbiamo trovato per tale directory
 	nl=`wc -l < /tmp/creato-$count`
 	
 	#stampiamo su standard output il numero di file trovati
 	echo Numero di file trovati per la gerarchia $i = $nl
 	
 	#se non è vuoto
 	if test $nl -ne 0
 	then
 		#chiamata C
 		echo Chiamata C: `cat /tmp/creato-$count`
 	fi
 	
 	#cancelliamo il file temporaneo
 	rm /tmp/creato-$count
 	
 	#incrementiamo la variabile count
 	count=`expr $count + 1`
 	
done

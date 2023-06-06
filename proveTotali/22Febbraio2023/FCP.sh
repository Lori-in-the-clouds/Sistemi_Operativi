#!/bin/sh

#controllo il numero di parametri, controllo lasco
case $# in
	0|1|2) #errore, numero di parametri non corretto
		echo Errore: numero di parametri non corretto
		exit 1;;

	*) ;; #ok
esac

 #controllo che il parametro $1 sia un numero strettamente positivo
 case $1 in
 	*[!0-9]*)
 		echo Errore: parametro $1 non numerico o non positivo
 		exit 2;;	
 	0)
 		echo Errore: parametro $1 uguale a 0
 		exit 3;;
 esac
 
 #controllo che $1 sia un numero srettamente minore di 500
 if test $1 -ge 500
 then
 	echo Errore: parametro $1 maggiore o uguale di 500
 	exit 4
 fi
 
 #inseriamo nella varaibile X il valore del parametro $1
 X=$1
 
 #eseguiamo uno shift 
 shift 
 
 #controlliamo che i restanti parametri siano nomi assoluti di directory e che siano attravesabili(non richiesto esplicitamente dalla consegna, ma necessario)
 for i
 do
	 #controlliamo che sia il nome assoluto di una gerarchia e quindi una directory attraversabile
	 case $i in
	 	/*)
	 		if test ! -d $i -o ! -w $i	#se nome assoluto, controlliamo sia una dir attraversabile
	 			then 
	 				echo Errore: $i non directory non attraversabile
	 				exit 5
	 		fi;;
	 	*)
	 		echo Errore: $i non nome assoluto; exit 6;;
	 esac
 done
 
 #se arriviamo qui abbiamo finito i controlli sui parametri
 #quindi possiamo passare a settare la variabile di ambiente PATH
 
 PATH=`pwd`:$PATH
 export PATH
 
 #creiamo un file temporaneo
 > /tmp/creato
 
 #chiamate ricorsive
 for G 
 do 
 	#chiamata ricorsiva
 	FCR.sh $G $X /tmp/creato
 done
 
 #otteniamo il numero di file trovati nella parte ricorsiva
 nl=`wc -l < /tmp/creato`
 
 #controlliamo che il numero di file sia maggiore o uguale a 2
 if test $nl -ge 2
 then
 	#chiamata alla parte C
 	#echo DEBUG-Chiamata alla parte C: `cat /tmp/creato`
 	main `cat /tmp/creato`
 fi
 
 #elimino il file temporaneo
 rm  /tmp/creato


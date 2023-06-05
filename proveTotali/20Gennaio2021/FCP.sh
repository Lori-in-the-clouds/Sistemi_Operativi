#!/bin/sh

#controllo il numero di parametri, controllo (stretto/lasco)
case $# in
	0|1|2) #errore, numero di parametri non corretto
		echo Errore: numero di parametri non corretto
		exit 1;;
	*);; #vuoto
esac

 #controlliamo che sia il nome assoluto di una gerarchia e quindi una directory attraversabil(controllo non richiesto dalla consegna ma necessaria)
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
 
 G=$1
 
 #eseguiamo un shit
 shift
 
 #controlliamo che i restanti parametri siano nomi di file
 for i 
 do 
 	 #controlliamo che sia il nome semplice di un file, 
	case $i in
 		*/*) 
 			echo Errore: in una stringa che sara\' usata come estensione non ci puo\' essere uno slash
 			exit 4;;
 		*) ;; #ok
 	esac
done

 #se arriviamo qui abbiamo finito i controlli sui parametri
 #quindi possiamo passare a settare la variabile di ambiente PATH
 
 PATH=`pwd`:$PATH
 export PATH

#chiamata ricorsiva

FCR.sh $G $*




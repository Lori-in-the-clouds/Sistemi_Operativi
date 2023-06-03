#!/bin/sh

#controllo il numero di parametri, controllo stretto
case $# in
	3);; #vuoto

	*) #errore, numero di parametri non corretto
		echo Errore: numero di parametri non corretto
		exit 1;;
esac

G=$1
shift

#controlli sulle due stringhe
 for i in $*
 do
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
FCR.sh $G $1 $2
 
 

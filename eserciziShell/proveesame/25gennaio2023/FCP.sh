#!/bin/sh

#controllo il numero di parametri, controllo stretto
case $# in
	3);; #ok

	*) #errore, numero di parametri non corretto
		echo Errore: numero di parametri non corretto
		exit 1;;
esac

 #controlliamo che sia il nome assoluto di una gerarchia e quindi una directory attraversabile
 case $1 in
 	/*)
 		if test ! -d $1 -o ! -w $1	#se nome assoluto, contrlliamo sia una dir attraversabile
 			then 
 				echo Errore: $1 non directory non attraversabile
 				exit 2
 		fi;;
 	*)
 		echo Errore: $1 non nome assoluto; exit 3;;
 esac
 
 G=$1 #inseriamo nella varaibile G il parametro $1
 #eseguiamo uno shift
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
 
 #controlliamo che le due stringhe passate non siano uguali
 if test $1 = $2
 	then echo Errore: le due stringhe $1 e $2 sono uguali e questo non è corretto; exit 5
 fi
 
 E1=$2 #inseriamo nella variabile E1 il parametro $2
 E2=$3 #inseriamo nella variabile E2 il parametro $3
 
 #se arriviamo qui abbiamo finito i controlli sui parametri
 #quindi possiamo passare a settare la variabile di ambiente PATH
 
 PATH=`pwd`:$PATH
 export PATH
 
 
 #chiamata ricorsiva
 FCR.sh $G $E1 $E2 
 
 echo ho finito
 

  
 
 

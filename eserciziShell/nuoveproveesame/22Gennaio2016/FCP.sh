#!/bin/sh


#controllo il numero di parametri, controllo stretto
case $# in
	2);; #vuoto

	*) #errore, numero di parametri non corretto
		echo Errore: numero di parametri non corretto
		exit 1;;
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

G=$1


 
 #controllo che il parametro $2 sia un numero strettamente positivo
 case $2 in
 	*[!0-9]*)
 		echo Errore: parametro $2 non numerico o non positivo
 		exit 4;;	
 	0)
 		echo Errore: parametro $2 uguale a 0
 		exit 5;;
 esac
 
N=$2

#cotrollo che il parametro N sia maggiore o uguale a 2
if test $N -lt 2
then
	echo Errore: parametro $N non è maggiore o uguale a 2
	exit 6
fi 

 #se arriviamo qui abbiamo finito i controlli sui parametri
 #quindi possiamo passare a settare la variabile di ambiente PATH
 
 PATH=`pwd`:$PATH
 export PATH
 
 
 #chiamata al file ricorsivo
 FCR.sh $G $N 
 
 
 
 

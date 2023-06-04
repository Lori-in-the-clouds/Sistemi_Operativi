#!/bin/sh

#controllo il numero di parametri, controllo lasco
case $# in
	0|1|2) #errore, numero di parametri non corretto
		echo Errore: numero di parametri non corretto
		exit 1;;
	*);; #vuoto
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
 
 #controllo che il parametro $1 sia un numero dispari
 control=`expr $1 % 2`
 if test $control -eq 0
 then
 	echo Errore: parametro $1 non è dispari; exit 4
 fi
 
 X=$1 #assegniamo alla variabile X il valore $2
 
 #eseguiamo una shift 
 shift
 
 #controllo che i restanti parametri siano nomi assoluti di directory (controllo che siano anche attravesabili, non richiesto dalle specifiche ma necessario)
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
 
 #eseguiamo le chiamate ricorsive
 for G 
 do
 	#chiamata ricorsiva
 	FCR.sh $G $X
 done
 
 #ho finito
 

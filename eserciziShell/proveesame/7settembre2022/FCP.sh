#!/bin/sh

#controllo il numero di parametri, controllo lasco
case $# in
	0 | 2 ) #errore, numero di parametri non corretto
		echo Errore: numero di parametri non corretto
		exit 1;;

	*) ;; #ok
esac

#controllo che il primo parametro sia un numero strettamente positivo
 case $1 in
 	*[!0-9]*) 
 		echo Errore: parametro $1 non numerico o non positivo
 		exit 2;;	
 	0)
 		echo Errore: parametro $1 uguale a 0
 		exit 3;;
 	*) 
 		#controllo che sia dispari
 		prov=`expr $1 % 2`
 		if test $prov -eq 0
 			then 
 				echo Errore: parametro $1 pari
 				exit 4
 		fi;;
 esac
 
 #inseriamo nella variabile X il primo parametro
 X=$1
 #eseguiamo uno shift
 shift
 
 #controlliamo i restanti parametri 
 for i in $*
 do 
 
	 #controlliamo che sia il nome assoluto di una gerarchia e quindi una directory attraversabile
	 case $i in
	 	/*)
	 		if test ! -d $i -o ! -w $i	#se nome assoluto, contrlliamo sia una dir attraversabile
	 			then 
	 				echo Errore: $1 non directory o non attraversabile
	 				exit 2
	 		fi;;
	 	*)
	 		echo Errore: $i non nome assoluto; exit 3;;
	 esac
done

#se arriviamo qui abbiamo finito i controlli sui parametri
#quindi possiamo passare a settare la variabile di ambiente PATH
 
PATH=`pwd`:$PATH
export PATH

#chiamata ricorsiva
for G in $*
do 
	#creiamo un file temporaneo
	> /tmp/count
	FCR.sh $G $X /tmp/count
	
	#parametro che mi indica il numero di file trovati
	nl=`wc -l < /tmp/count`
	
	#controlliamo che sia diverso da 0, cioè ha trovato almeno una soluzione
	if test $nl -ne 0
	then
		echo Invcocazione C:
		echo Nomi assoluti dei file trovati per la gerarchia $G:
		cat /tmp/count
	fi
done



 
 
 

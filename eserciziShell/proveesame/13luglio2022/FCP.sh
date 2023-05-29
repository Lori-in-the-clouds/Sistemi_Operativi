#!/bin/sh

#controllo il numero di parametri, controllo lasco
case $# in
	0 | 1 | 2 | 3) #errore, numero di parametri non corretto
			echo Errore: numero di parametri non corretto
			exit 1;;
	*);; #ok
esac

 #controlliamo che il primo parametro sia il nome assoluto di una gerarchia e quindi una directory attraversabile
 case $1 in
 	/*)
 		if test ! -d $1 -o ! -w $1	#se nome assoluto, contrlliamo sia una dir attraversabile
 			then 
 				echo Errore: $1 non directory o non attraversabile
 				exit 2
 		fi;;
 	*)
 		echo Errore: $1 non nome assoluto; exit 3;;
 esac
 
 #nominiamo il primo parametro 
 G=$1
 shift 
 #controllo che il secondo parametro sia un numero strettamente positivo
 case $1 in
 	*[!0-9]*) 
 		echo Errore: parametro $1 non numerico o non positivo
 		exit 4;;	
 	0)
 		echo Errore: parametro $1 uguale a 0
 		exit 5;;
 esac
 
 #nominiamo il secondo parametro
 L=$1
 shift
 
for i in $*
do
	 #controllo che il parametro $ sia un singolo carattere
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
 

#creiamo un file temporaneo
> /tmp/count
#chiamata ricorsiva
FCR.sh $G $L /tmp/count $*

#parametro che mi indica il numero di file trovati
nl=`wc -l < /tmp/count`

echo Numero di file trovati: $nl
echo Invocazione parte C: 
echo File:`cat /tmp/count`
echo Valore di L: $L
echo Valori di tutti i parametri passati:
for i in $*
do 
	echo $i
done
#cancelliamo il file temporaneo
rm /tmp/count

echo ho finito

	 

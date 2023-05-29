#!/bin/sh

#controllo numero di parametri
case $# in
	1) ;; #ok
	*) echo ERRORE: numero di parametri passato errrato; exit 1;;
esac

#controllo sul primo parametro
case $1 in
	/*)
		if test ! -d $1 -o ! -x $1
			then echo ERRORE: parametro $1 non è una directory oppure non è eseguibile; exit 2
			
		fi;;
	*)
		echo ERRORE: parametro $1 non in forma assoluta; exit 3;;
esac

#abbiamo finito il controllo sui parametri 

#settiamo la variabile path
PATH=`pwd`:$PATH
export PATH

#valore che andremo a scrivere nel file temporaneo
conta=0
fase=A

#scriviamo il valore di conta (cioè 0) nel file temporaneo
echo $conta > /tmp/tmpContaLivelli

#invochiamo la fase A
FCR.sh $1 $conta $fase

read tot < /tmp/tmpContaLivelli
#stampiamo il numero di livelli totali della gerarchia
echo Il numero di livelli totali della gerarchia = $tot

#invochiamo la fase B
fase=B

#chiediamo un numero compreso tra 1 e $tot
echo Inserire un numero compreso tra 1 e $tot:

#leggiamo in input un valore 
read X

#valutiamo se tale valore è un numero positivo 
case $X in
	*[!0-9]*) echo ERRORE: il valore passato non è un numero oppure è negativo; exit 4;;
	0) echo ERRORE: il valore passato è uguale a 0; exit 5;;
esac

#controlliamom se il valore sia minore di $tot
if test $X -le $tot
	then 
		FCR.sh $1 $conta $fase $X
		
fi

echo ho finito











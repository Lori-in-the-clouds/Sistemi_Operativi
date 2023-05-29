#!/bin/sh

#controlliamo il numero di parametri
case $# in
	3) ;; #ok
	*) echo ERRORE: numero di parametri passati non corretto; exit 1;;
esac

#controlliamo il primo valore
case $1 in
	#controlliamo che $1 sia in forma assoluta
	/*)
		#controlliamo che sia una directory e che sia eseguibile 
		if test ! -d $1 -o ! -x $1
			then echo ERRORE: parametro $1 non è una directory oppure non è eseguibile; exit 2;
		fi;;
	*) 
		echo ERRORE: parametro passato $i non in forma assoluta; exit 3;;
esac

#controlliamo che il terzo parametro sia un numero strettamente positivo
case $3 in
	*[!0-9]*) echo ERRORE: parametro passato non è un numero oppure è negativo; exit 4;;
	0) echo ERRORE: parametro passato è uguale a 0; exit 5;;
esac

#abbiamo effetuato tutti i controlli
#settiamo la variabile path
PATH=`pwd`:$PATH
export PATH

#creiamo un file temporaneo
> /tmp/count$$

#FASE A
FCR.sh $1 $2 /tmp/count$$

#parametro che mi indica il numero di directory trovate nella fase A
nl=`wc -l < /tmp/count$$`
echo Il numero di directory trovate sono: $nl

if test $nl -gt $3
	then
		#chiediamo all'utente un numero X compreso tra 1 e N
		echo Inserire un numero compreso tra 1 e $3:
		read X
		#controlliamo che il parametro X sia un numero strettamente positivo
		case $X in
			*[!0-9]*) echo ERRORE: parametro passato non è un numero oppure è negativo; exit 4;;
			0) echo ERRORE: parametro passato è uguale a 0; exit 5;;
		esac
		#controlliamo che sia minore o uguale a N
		if test $X -le $3
			then 
				control=1
				for i in `cat /tmp/count$$`
				do
					#siamo giunti alla directory selezionata
					if test $control -eq $X
						then 
							echo Directory selezionata: $i
							#posizioniamoci in tale directory 
							cd $i
							#navighiamo tale directory
							for j in *
							do
								#controlliamo che sia un file leggibile
								if test ! -f $j -o ! -r $j
									then continue
								fi
								case $j in
									*.$2)  
										echo file trovato:`pwd`/$j
										echo stampiamo la prima linea del file:
										head -1 < $j
									;;
								esac
							done
				
					fi
					control=`expr $control + 1`
								
				done				
		fi
fi
echo ho finito	
	
	
	
	
	
	


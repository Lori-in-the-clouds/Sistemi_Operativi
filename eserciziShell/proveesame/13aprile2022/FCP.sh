#!/bin/sh

#controlliamo il numero di parametri
case $# in
0 | 1 | 2) echo ERRORE: numero di parametri passato errato; exit 1;;
*) ;; #ok
esac
#controlliamo che il primo parametro sia un numero strettametne positivo
case $1 in
	#controlliamo che sia un numero e che non sia negativo
	*[!0-9]*) echo ERRORE: parametro $1 non è un numero oppure è negativo; exit 2;;
	0) echo ERRORE: parametro passato è uguale a 0; exit 3;;
esac

#inseriamo il primo parametro all'interno di una variabile
var1=$1
#eseguiamo lo shift
shift

#controlliamo i restanti parametri
for i in $*
do 
	#controlliamo che siano nomi assoluti
	case $i in 
	/*) 
		if test ! -d $i -o ! -x $i
			then echo ERRORE: parametro $i passato non è una directory oppure non è eseguibile; exit 4
		fi;;
	*) echo ERRORE: parametro passato $i non è in forma assoluta; exit 5;;
	esac
done

#controlli effettuati

#settiamo la variabile path
PATH=`pwd`:$PATH
export PATH

#creiamo una variabile 
n=1
#chiamata a FCR.sh
for i in $*
do
	#creiamo un file temporaneo per ogni gerarchia
	> /tmp/count$$-$n
	FCR.sh $i $var1 /tmp/count$$-$n
	#variabile che mi conta quanti file si sono trovati per tale gerarchia 
	nl=`wc -l < /tmp/count$$-$n`
	echo Numero di file trovati per la gerarichia $i: $nl
	n=`expr $n + 1`
done

#abbiamo terminato tutte le Q fasi
c=2

#ciclo che ci permette di passare da un file temporaneo ad un altro
while test $c -lt $n
	do	
		#ciclo che ci estrapola ciasuna riga del file temporaneo (non il primo)
		for i in `cat /tmp/count$$-$c`
		do	
			#ciclo che ci estrapola ciasuna riga del primo file temporaneo
			for j in `cat /tmp/count$$-1`
			do
				if diff $j $i > /dev/null 2>&1
					then echo Non si sono trovate differenze tra $j e $i
					else echo Si sono trovate differeze tra $j e $i
				fi
			done
		
		done
		#incrementiamo la variabile n
		c=`expr $c + 1`
	done

echo ho finito

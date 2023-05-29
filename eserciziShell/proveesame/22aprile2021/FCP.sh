#/!bin/sh

#controlliamo che il numero di parametri passato sia corretto
case $# in
	0 | 1 | 2 | 3) echo ERRORE: numero di parametri passato non corretto; exit 1;;
	*) ;; #ok
esac
#inseriamo nelle variabili W e S rispettivamente il primo e il secondo parametro
W=$1
S=$2
#contolliamo che il primo parametro sia un nuemero strettamente positivo
case $W in
	*[!0-9]*) echo ERRORE: parametro $W passato non è un numero positivo; exit 2;;
	0) echo ERRORE: parametro passato uguale a 0; exit 3;;
	*) ;; #ok
esac

count=0

#controlliamo i restanti parametri 
for i in $*
do
	if test $count -ge 2
	then
		#controllo che sia scritta in forma assoluta
		case $i in
		#controllo che sia una directory e che sia attraversabile
		/*) if test ! -d $i -o ! -x $i
			then echo ERRORE: parametro $i non è una directory oppure non è attraversabile; exit 3
		    fi;;
		*) echo ERRORE: parametro passato $i non è in forma assoluta: exit 4;;
		esac
	fi
	#incrementiamo la variabile count
	count=`expr $count + 1`
done

#abbiamo effettuato tutti i controlli, setiamo la variabile PATH
PATH=`pwd`:$PATH
export PATH
#creiamo un file temporaneo
> /tmp/nomiAssoluti$$

count=0
for i in $*
do
	if test $count -ge 2
	then
		G=$i
		if test -d $G -a -x $G
			then FCR.sh $G $W $S /tmp/nomiAssoluti$$
		fi
	fi
	count=`expr $count + 1`
done

#parametro che contine il numero di righe del file temporaneo
nl=`wc -l < /tmp/nomiAssoluti$$`
#stampaimo il numero il numero di directory trovate
echo Numero di directory trovate: $nl

if test $nl -ge $W
	then
		#caso in cui il nuemro di linee sia maggiore o uguale a $W
		echo Lorenzo inserisci un valore compreso tra 1 e $W:
		#leggiamo il valore in input
		read X
		#controlliamo che il valore X sia un numero strettamente positivo
		case $X in
			*[!0-9]*) echo ERRORE: Il valore passato non è un numero oppure non è positivo; exit 5;;
			0)echo ERRORE: Il Valore passato è uguale a zero; exit 6;;
		esac
		#controlliamo che il valore sia minore di W
		if test $X -gt $W
		then echo ERRORE: Valore $X non è minore di $W; exit 7
		fi

		#stampiamo la X-esima directory
		count=1
		for i in `cat /tmp/nomiAssoluti$$`
		do
			if test $count -eq $X
			then 
				echo $i; break
			fi
			count=`expr $count + 1`
		done
	else cat /tmp/nomiAssoluti$$
fi
echo Ho finito



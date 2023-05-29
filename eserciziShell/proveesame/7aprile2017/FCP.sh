#/!bin/sh

#controlliamo il numero di parametri
case $# in
	0 | 1 | 2) echo ERRORE: numero di parametri passati non corretto; exit 1;;
	*);; #nuemro di parametri passati corretto
esac

#controlliamo che il primo parametro sia un nome di un file in forma relativa semplice 
case $1 in
	/*) ERRORE: parametro $1 scritto in forma assoluta e non in forma relativa semplice; exit 2;;
	*/*)ERRORE: parametro $1 scritto in forma relativa e non in froma relativa semplice; exit 3;;
	*) ;; #ok
esac
#variabile utilizzata per valutare i restanti parametri
count=0
#controllo ulteriori parametri
for i in $*
do
	if test $count -ne 0
	then 
		#controllo che sia in forma assoluta
		case $i in
		#controllo che sia una directory e che sia eseguibile
		/*) if test ! -d $i -o ! -x $i
			then ERRORE: parametro $i non è una directory o non è eseguibile; exit 6
		    fi;;
		*) echo ERRORE: parametro $i non in forma assoluta; exit 5;;
		esac
	fi
	count=`expr $count + 1`
done
#tutti i controlli sono stati effettuati
#settiamo la variabile path
PATH=`pwd`:$PATH
export PATH
#creiamo un file temporaneo 
> /tmp/count$$
#variabile che utilizzo per non valutare il primo parametro
count=0
for i in $*
do
	if test $count -ne 0
	then 
		FCR.sh $i /tmp/count$$ $1
	fi
	count=`expr $count + 1`	
done

#parametro che mi indica il numero di file trovati
nl=`wc -l < /tmp/count$$`
echo Numero di file creati globalmente: $nl

#ciclo dove valutiamo tutti i nomi assoluti dei file creati
for i in `cat /tmp/count$$`
do	
	#stampiamo il nome assoluto del file 
	echo Nome file creato:$i
	#staimpiamo la prima riga del file 
	echo Stampiamo la prima riga di tale file:
	echo `head -1 < $i`
	#stampiamo l'ultima riga del file
	echo Stampiamo l\' ultima riga del file:
	echo `tail -1 < $i`
done

#cancelliamo il file temporaneo
rm /tmp/count$$

echo ho finito

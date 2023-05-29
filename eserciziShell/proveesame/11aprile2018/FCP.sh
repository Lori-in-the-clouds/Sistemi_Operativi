#/!bin/sh

#controllo il numero di parametri, controllo lasco
case $# in
	0 | 1 ) echo Errore: numero di parametri passato errato; exit 1;;
	*);; #ok
esac
#count e limit mi servono per valutare il primo valore e i resanti valori
count=0
limit=`expr $# - 1`
for i in $*
do
	if test $count -gt $limit
	then break
	fi
	
	if test $count -eq 0
	then 
		case $i in
		#controllo se $1 è un numero positivo
		*[!0-9]*) echo ERRORE: parametro $i  passato non è un numero o non è positivo; exit 2;;
		#controllo se $1 è diverso da 0
		O) echo ERRORE: primo numero uguale a 0; exit 3;;
		esac
	else 
		case $i in
		#controlliamo che sia una directory e che sia eseguibile
		/*) if test ! -d $i -o ! -x $i
		 	then echo ERRORE: directory $i non esiste oppure non è eseguibile; exit 4
		    fi;;
		*)echo ERORRE: parametro $i non è in valore assoluto; exit  5;;
		esac
	fi
	count=`expr $count + 1`
done


#settiamo la variabile path
PATH=`pwd`:$PATH
export PATH

#entriamo nella parte ricorsiva
count=0
#creiamo file temporaneo
> /tmp/count$$

#eseguimo le N fasi
for i in $*
do 
	if test $count -ne 0
	then 
		FCR.sh $i $1 /tmp/count$$
	fi
	count=`expr $count + 1`
done

#numero di file trovati
echo Numeri di file trovati:
nl=`wc -l < /tmp/count$$`

for i in `cat /tmp/count$$`
do	
	echo Nome assoluto:
	echo $i
	echo Contenuto del file:
	cat $i
done
#cancelliamo il file temporaneo
rm /tmp/count$$
echo ho finito

#/bin/sh

#controlliamo il numero di parametri
case $# in
	0 | 1 | 2 | 3) echo ERRORE: numero di parametri passati errato; exit 1;;
	*);; #ok
esac

#variabile che mi indica fino a dove controllare i parametri Q
var=`expr $# - 2`
count=1

for i in $*
do
	if test $count -le $var
	then
		#controllo che sia in forma assoluta
		case $i in
			/*)	#controllo che sia il nome di una directory e che sia eseguibile
				if test  ! -d $i -o ! -x $i
					then echo ERRORE: parametro passato noo è una directory; exit 2
				fi;;
			*) echo ERORRE: parametro $i non è in forma assoluta oppure non eseguibile; exit 3;;
		esac

	else
		#controllo i restanti parametri, che devono essere numeri strettamente positivi
		case $i in
			*[!0-9]*) echo ERRORE: parametro $i non è un numero oppure è negativo; exit 4;;
			0) echo ERRORE: parametro passato è uguale a zero; exit 5;;
			*) #i restanti parametri sono corretti,salviamoli all'interno di variabili
				if test $count -ne $#
					then H=$i
					else M=$i
				fi
		esac
	fi

	count=`expr $count + 1`
done

#tutti i controlli sono stati effettuati settiamo la variabile PATH
PATH=`pwd`:$PATH
export PATH

#chiamata al file ricorsiva nelle Q fasi
count=0

#creiamo un file temporaneo
> /tmp/cont$$
for i in $*
do
	if test  $count -lt $var
	#chiamata ricorsiva
	then FCR.sh $i $H $M /tmp/cont$$
	fi
	#incrementiamo il contatore
	count=`expr $count + 1`
done
echo Invocazione file C:
echo FIle trovati:
cat < /tmp/cont$$
echo numero M: $M

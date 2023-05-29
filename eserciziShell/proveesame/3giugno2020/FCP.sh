#/!bin/sh

#controllo che il numero di parametri sia maggiore o uguale a 2
case $# in
	0 | 1) echo ERRORE: numero di parametri passati errato; exit 1;;
	*);; #ok
esac

count=0

for i in $*
do
	if test $count -eq 0
	then
		case $i in
		?);; #ok
		*) echo ERRORE: primo parametro non è un singolo carattere; exit 2;;
		esac
	else
		case $i in
		/*)
			if test ! -d $i -o ! -x $i
			then
				echo ERRORE: parametro $i non è una directory oppure non è eseguibile; exit 3
			fi;;
		*) echo ERRORE: parametro $i non in forma assoluta; exit 4;;
		esac
	fi
	count=`expr $count + 1`
done

#ho finito i controlli,settiamo la variabile PATH
PATH=`pwd`:$PATH
export PATH

#creiamo un file temporaneo
> /tmp/nomiassoluti$$

count=0
#chiamata ricorsiva
for i in $*
do
	if test $count -ne 0
	then
		if test -d $i -a -x $i
		then FCR.sh $i $1 /tmp/nomiassoluti$$
		fi
	fi
	count=`expr $count + 1`
done

#stampiamo il numero di directory trovate
nl=`wc -l < /tmp/nomiassoluti$$`
echo Numero di directory trovate: $nl

for i in `cat /tmp/nomiassoluti$$`
do
	echo Nome directory:$i
	echo Elena vuoi visualizzare il contenuto di tale directory?
	#leggiamo la risposta
	read answer
	case $answer in
		#se dice si 
		y* | Y* | yes | YES) ls -la $i;;
		*);;
	esac
done

echo ho finito

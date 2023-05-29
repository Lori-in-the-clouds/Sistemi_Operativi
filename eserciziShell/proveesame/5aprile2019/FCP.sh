#/!bin/sh

#controllo che il numero di parametri sia maggiore o uguale a 2
case $# in
	0 | 1) echo ERRORE: numero di parametri passati non corretto; exit 1;;
	*);;
esac

#parametro che utilizziamo per individuare l'ultimo parametro passato
limit=`expr $# - 1`
count=0

for i in $*
do
	#controllo che no sia arrivato l'ultimo parametro
	if test $count -ne $limit
	then 
		case $i in
		#controllo che $i sia una directorye e che sia eseguibile
		/*) if test ! -d $i -o ! -x $i
		    then 
			echo ERRORE: parametro passato $i non è una directory oppure non è eseguibile; exit 2
	            fi;;
		*) echo ERORRE: parametro $i passato non è in forma assoluta; exit 2;;
		esac
	fi
	#incrementiamo la variabile count
	count=`expr $count + 1`
done


#controlli effettuati, settiamo la variabile path
PATH=`pwd`:$PATH
export PATH

#creiamo un file tempraneo per i nomi dei file
> /tmp/file$$


#chiamata N ricorsiva
count=0

for i in $*
do
	if test $count -ne $limit
		then FCR.sh $i /tmp/file$$
	fi
	count=`expr $count + 1`
done

#contiamo quanti file abbiamo trovato
nl=`wc -l < /tmp/file$$`
nl=`expr $nl / 2`
echo Abbiamo trovato $nl file

count=1
for i in `cat /tmp/file$$`
do
	#ricaviamo il nome di un file e il suo numero di caratteri
	tmp=`expr $count % 2`
	if test $tmp -ne 0
		then numfile=$i
		echo Vuoi ordinare $numfile in ordine alfabetico?
		read var1
			case $var1 in
				y* | yes | YES | Y*)
					echo `sort $i` > $i
					cat $i;;
				*);;
			esac
		else
			numc=$i
			echo Numero di caratteri del file $numc
		fi
		count=`expr $count + 1`
done








echo ho finito

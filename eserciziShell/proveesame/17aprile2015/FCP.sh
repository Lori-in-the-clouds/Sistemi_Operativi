#/!bin/sh

#controlliamo che il numero di parametri passati sia maggiore di 2
case $# in
	0 | 1) echo ERRORE: numero di parametri passato non corretto; exit 1;;
	*)
		#controlliamo che il primo parametro sia un numero positivo
		case $1 in
			*[!0-9]*) echo ERRORE: parametro $1 non è un numero oppure non è positivo; exit 2;;
			*)
				if test $1 -eq 0
				then echo ERRORE: parametro $1 è uguale a zero; exit 3
				fi;;
		esac
		#parametro che utilizzo per saltare il primo
		count=0 
		#controlliamo che i restanti parametri siano nomi assoluti di directory
		for i in $*
		do
			if test $count -ne 0
			then 
				 case $i in
					#controlliamo che la director sia in forma assoluta e sia eseguibile
					/*)
					    if test ! -d $i -a -x $i
						then echo ERRORE: il parametro passato $i non è una directory oppure non è eseguibile; exit 4
					    fi;;
					*) echo ERRORE: parametro  $i non è in forma assoluta; exit 5;;
				 esac
			fi
			count=`expr $count + 1`
		done;;
esac

#ho finito i controlli
#settiam la variabile path

PATH=`pwd`:$PATH
export PATH

#creiamo un file temporaneo dove inseriremo i vari file che soddifano le richieste
> /tmp/cont$$

#chiamata al file ricorsivo N volte
count=0
for i in $*
do
	if test $count -ne 0
		then FCR.sh $i $1 /tmp/cont$$
	fi
	count=`expr $count + 1`
done

#stampiamo il numero di file trovati
echo Numero di file trovati: `wc -l < /tmp/cont$$`

#variabile lista in cui inseriremo i file con i relativi numeri 
LIST=

for i in `cat /tmp/cont$$`
do
	echo Il file considerato è $i
	echo Inserire un numero positivo strettamente minore di $1:
	read var
	case $var in
		*[!0-9]*) echo ERRORE: parametro passato non è un numero oppure è negativo; rm /tmp/cont$$; exit 6;;
		0) echo ERRORE: parametro passato uguale a 0; rm /tmp/cont$$; exit 7;;
		*)
			if test $var -ge $1
				then echo ERRORE: parametro passato è maggiore o uguale di $1; rm /tmp/cont$$; exit 8
			fi;;
	esac
	echo Stampo la linea numero $var:
	echo ` head -$var $i | tail -1`
done
echo finito

#/!bin/sh

#controlliamo che il numero di parametri dato sia 2; controllo stretto
case $# in
	2) case $1 in
		/*) #controlliamo che sia una directory esistente e che sia eseguibile
			if test ! -d $1 -o ! -w $1
				then echo ERRORE: parametro $1 non è una directory esistente oppure non è eseguibile; exit 4
			fi;;
		*) echo ERRORE: parametro $1 non in forma assoluta; exit 1;; 
	   esac
	   if test $2 -lt 0
		then echo Errore: parametro $2 non strettamente positivo; exit 2
	   fi
	;;
	*) echo ERRORE: numero di parametri passato errato; exit 3;;
esac

#tutti i controlli sono stati effettuati, settiamo la variabile path
PATH=`pwd`:$PATH
export PATH

#creiamo un file temporaneo
> /tmp/dir$$

#creiamo un secondo file temporaneo
> /tmp/file$$

#invochiamo il file ricorsivo
FCR.sh $1 $2 /tmp/dir$$ /tmp/file$$

#stampiamo i nomi dei file che abbiamo trovato 
echo Stampiamo le directory che soddisfano le condizioni richeste:
cat /tmp/dir$$

#lista in cui inseriamo il nome del file con il relativo numero 
LIST=
for i in `cat /tmp/file$$`
do
	echo Inserire un numero intero positivo minore o uguale di $2:
	read var1
	case $var1 in
		*[!0-9]*) echo ERRORE: parametro passato non numero o non positivo; rm /tmp/dir$$; rm /tmp/file$$; exit 5;;
		*) if test $var1 -eq 0
			then echo ERRORE: parametro passato non uguale a 0;rm /tmp/dir$$; rm /tmp/file$$; exit 6
		   fi
		   if test $var1 -gt $2
		        then echo ERRORE: parametro passato maggiore di $2; rm /tmp/dir$$; rm /tmp/file$$; exit 7
		   fi;; 
	esac
	LIST="$LIST,$i,$var1"
done

echo $LIST
rm /tmp/dir$$
rm /tmp/file$$

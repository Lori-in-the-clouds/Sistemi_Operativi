#!/bin/sh

#controllo il numero di parametri, controllo lasco
case $# in
	0 | 1 | 2)
			#errore, numero di parametri non corretto
			echo Errore: numero di parametri non corretto
			exit 1;;
	*) ;; #ok 
esac

#controllo che il parametro $1 sia un numero strettamente positivo
 case $1 in
 	*[!0-9]*) 
 		echo Errore: parametro $1 non numerico o non positivo
 		exit 2;;	
 	0)
 		echo Errore: parametro $1 uguale a 0
 		exit 3;;
 	*) 
 		limit=500
 		if test $1 -ge $limit
 			then Errore: parametro $1 maggiore o uguale a 500; exit 4
 		fi;;
 esac
 
 #salviamo nella variabile X il primo parametro
 X=$1
 
 #eseguiamo uno shift
 shift 
 
 #controlliamo i restanti parametri
 for i in $*
 do
 	 #controlliamo che sia il nome assoluto di una gerarchia e quindi una directory attraversabile
	 case $i in
	 	/*)
	 		if test ! -d $i -o ! -w $i	#se nome assoluto, controlliamo sia una dir attraversabile
	 			then 
	 				echo Errore: $i non directory non attraversabile
	 				exit 5
	 		fi;;
	 	*)
	 		echo Errore: $i non nome assoluto; exit 6;;
	 esac
done

#se arriviamo qui abbiamo finito i controlli sui parametri
#quindi possiamo passare a settare la variabile di ambiente PATH
 
PATH=`pwd`:$PATH
export PATH
 
#creiamo un file temporaneo
> /tmp/count
 
#chiamata ricorsiva
for G in $*
do 
	
	FCR.sh $G $X /tmp/count
done
#parametro che mi indica il numero di righe del file e quindi il numero di file trovati nella parte ricorsiva
nl=`wc -l < /tmp/count`
if test $nl -ge 2
	then 
		echo Incovazione C
		cat /tmp/count
fi

#cancelliamo il file temporaneo
rm /tmp/count 

echo ho finito
 
 

#!/bin/sh

#controlliamo che il numero di parametri sia corretto
case $# in
	0 | 1) echo ERORRE: numero di parametri passato errato; exit 1;;
	*) ;; #ok
esac

#controllo i parametri 
for i in $*
do 
	case $i in
		/*)
			#controllo che sia una direcotory e che sia eseguibile
			if test ! -d $i -o ! -x $i
			then echo ERRORE: parametro $i non è una directory oppure non è eseguibile; exit 3
			fi;; 
			
		*) 	
			#il parametro passato non è in forma assoluta
			echo ERRORE: parametro $i non in forma assoluta; exit 4;;
	esac
done

#tutti i controlli sono stati effettuati

#settiamo la varibile path
PATH=`pwd`:$PATH
export PATH


n=1
for i in $*
do
	#creiamo un file temporaneo	
	> /tmp/count$$-$n
	FCR.sh $i /tmp/count$$-$n
	n=`expr $n + 1`
done

#fine della parte ricorsiva
n=1
for i in $*
do
	#riportiamo il nome della gerarchia 
	echo Nome della gerarchia: $i
	
	#riportiamo il numero di file trovati
	nf=`wc -l < /tmp/count$$-$n`
	echo Numero di file trovati per tale gerarchia: $nf
	
	#riportiamo sullo standar output i nome assoluto di ogni file 
	for j in `cat /tmp/count$$-$n`
	do
		#stampiamo il nome del file
		echo $j
		#chiediamo all'utente un numero X
		echo Inseririre un numero strettamente positivo:
		read X
		#controlliamo se tale valore è un numero e se è strettamente positivo
		case $X in
			*[!0-9]*) echo ERRORE: parametro passato non è un numero oppure non è positivo;exit 5;;
			0) echo ERRORE: parametro passato è uguale a 0; exit 6;;
		esac
		#stampiamo le prime X linee del file
		head -$X < $j
	done
	#eliminiamo il file temporaneo
	rm /tmp/count$$-$n
	#incrementiamo la variabile n
	n=`expr $n + 1`
done
	
echo ho finito

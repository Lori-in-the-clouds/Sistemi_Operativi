#!/bin/sh

#controlliamo il numero di parametri (controllo stretto)
case $# in
	3);; #OK
	*) echo ERRORE: nuemro di parametri passati non corretto; exit 1;;
esac

#controlliamo che il primo parametro sia un nome assoluto di una gerarchia
case $1 in
	/*) 
		if test ! -d $1 -o ! -x $1
		then echo ERRORE: parametro $1 non è una directory oppure non è attraversabile; exit 1
		fi;;
	*) 
		echo ERRORE: parametro passato $1 non in forma assoluta; exit 2;;
esac

#controlliamo che il secondo parametro sia una nome relativo semplice di un file
case $2 in
	*/)
		echo ERRORE: parametro passato $2 non in forma relativa semplice; exit 3;;
		
	*/*)
		echo ERRORE: parametro passato $2 non in forma relativa semplice; exit 4;;

	*) 	;; #ok	
		#if test ! -f $2 
		#then echo ERRORE: parametro $2 non è un file; exit 5
		#fi;;
esac

#controlliamo che il terzo parametro sia un numero strettamente positivo
case $3 in
	*[!0-9]*) echo ERRORE: parametro $3 non è un numero oppure è negativo; exit 6;;
	0) echo ERRORE: parametro passato è uguale a 0; exit 7;;
esac

#tutti in controlli sono stati effettuati, settamo la variabile path
PATH=`pwd`:$PATH
export PATH


#creiamo un file temporaneo
> /tmp/count-faseA$$

#chiamata ricorsiva FASE A
FCR.sh $1 $2 /tmp/count-faseA$$ 0

#parametro che mi indica il numero di file trovati
nf=`wc -l < /tmp/count-faseA$$`

#stampiamo il numero di file trovati
echo Numero di file trovati nella fase A: $nf

> /tmp/count-faseB$$
if test $nf -lt $3
	then 
		
		#chiamata ricorsiva FASE B
		FCR.sh $1 $2 /tmp/count-faseB$$ 1
		#parametro che mi indica il numero di file trovati
		nf=`wc -l < /tmp/count-faseB$$`
		#stampiamo il numero di file trovati
		echo Numero di file trovati nella fase B: $nf
		
		#fase C
		echo Eseguiamo la fase C:
		#creiamo un file temporaneo
		> /tmp/count-faseA$$

		#chiamata ricorsiva FASE A
		FCR.sh $1 $2 /tmp/count-faseA$$ 0

		#parametro che mi indica il numero di file trovati
		nf=`wc -l < /tmp/count-faseA$$`

		#stampiamo il numero di file trovati
		echo Numero di file trovati nella fase A: $nf
fi
echo  ho finito








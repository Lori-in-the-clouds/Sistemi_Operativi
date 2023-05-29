#!/bin/sh

#controllo sul numero di parametri (controllo stretto)
case $# in
	1) #controllo che il primo parametro sia in forma assoluta
		case $1 in
			/*) 	#controllo che sia una directory e che sia eseguibile
				if test ! -d $1 -o ! -x $1
					then ERROR: parametro $1 non è una directory oppure non è eseguibile; exit 2
				fi;; 
			*) echo ERROR: parametro non in forma assoluta; exit 3;;
		esac;;
	 
	*) echo ERROR: numero di parametri errato; exit 4;;
esac

#ho finito i controlli

#settiamo la variabile path
PATH=`pwd`:$PATH
export PATH

#variabile che utilizzerò per contare le gerarchie nel file ricorsivo
count=0
fase=A

#scriviamo il valore di count detro il file temporaneo 
echo $count > /tmp/tmpContaLivelli

#FASE A
#chiamata al file ricorsivo
FCR.sh $1 $count $fase

read tot < /tmp/tmpContaLivelli
echo Numero di gerarchie trovate: $tot

#FASE B
fase=B
echo Inserisci un valore  dispari compreso tra 1 e $tot
read X

#controlliamo se tale valore è un numero 
case $X in
	*[!0-9]*) echo ERROR: valore passato non è un numero oppure non è positivo; exit 5;;
	0) echo ERROR: valore passato è uguale a 0; exit 6;;
esac

#controlliamo che sia minore o uguale di $tot
if test $X -le $tot
	then 
		tmp=`expr $X % 2`
		if test $tmp -eq 1
			then 
				#chiamata ricorsiva
				FCR.sh $1 $count $fase $X
		fi
fi
				



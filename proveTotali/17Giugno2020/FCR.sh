#!/bin/sh

cd $1 #ci spostiamo nella directory in cui dobbiamo fare la ricerca per questa invocazione

control1=false #parametro che mi conrtolla che ci sia o meno una sottodirectory
control2=false #parametro che mi controlla che ci sia almeno un file che rispetti le specifiche
LISTA= #parametro che utilizzo per contenere il nome assoluto dei file che rispetano le specifiche
LISTADIR=

S=
for F in *
do
	#controllo che sia un file
	if test -f $F
	then
		#otteniamo la lunghezza in caratteri del file
		L=`wc -c < $F`

		#controlliamo che sia multiplo di B
		S=`expr $L % $2`
		
		if test $S -eq 0
		then
			LISTA="$LISTA $F"
			control2=true
		fi 
	fi
	
	if test -d $F
	then 
		control1=true
		dir=$F
	fi
done

if test $control1 = true -a $control2 = true
then
	#stampiamo il nome assoluto della directory
	echo `pwd`
	for F in $LISTA
	do
		#creiamo un file in tale sotto-directory
		> $F.Chiara
		
		#otteniamo la lunghezza in caratteri del file
		L=`wc -c < $F`
		echo Invoco parte C: $F $L $2
		main $F $2 $L
	done
fi

 #passiamo ora alla parte ricorsiva
 
 for i in *
 do
 	if test -d $i -a -x $i
 	then 
 		#chiamata ricorsiva cui passiamo come primo parametro il nome assoluto della directory
 		FCR.sh `pwd`/$i $2
 	fi
 done

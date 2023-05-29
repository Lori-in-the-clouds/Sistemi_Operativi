#!/bin/sh

#posizioniamoci nella directory corrente
cd $1

for i in *
do 
	#controllo che $i sia un file e che sia leggibile
	if test ! -f $i -o ! -r $i
		then continue
	fi
	
	#parametro che mi stabilisce la lunghezza il linee del mio file
	nl=`wc -l < $i`
	
	#controllo che $nl sia uguale a $2
	if test $nl -ne $2
		then continue
	fi
	
	#inseriamo il nome assoluto di tale file all'interno del file temporaneo
	echo `pwd`/$i >> $3
done

#chiamata ricorsiva
for i in *
do	
	#contorllo che sia una directory e che sia eseguibile
	if test -d $i -a -x $i
		#chiamata ricorsiva
		then FCR.sh $i $2 $3
	fi
done

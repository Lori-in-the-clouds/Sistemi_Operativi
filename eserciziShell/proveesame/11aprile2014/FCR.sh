#!/bin/sh

#posizioniamoci nella directory corrente
cd $1

for i in *
do
	#controlliamo che sia un file e che sia leggibile 
	if test ! -f $i -o ! -r $i
		then continue
	fi
	
	#parametro che mi indica il numero di linee del file
	nl=`wc -l < $i`
	
	#controlliamo che il file non sia vuoto
	if test $nl -eq 0
		then continue
	fi
	
	#parametro che mi indica quante righe incominciano con il carattere 'a'
	na=`grep '^a' $i | wc -l`
	
	#controlliamo che $na coincida con $nl
	if test $na -eq $nl
		then
			#inseriamo il nome assoluto del file dentro il file temporaneo
			echo `pwd`/$i >> $2
	fi
done	

#chiamata ricorsiva
for i in *
do 
	if test -d $i -a -x $i
	then 
		FCR.sh `pwd`/$i $2
	fi
done

#!/bin/sh

#posizioniamoci nella directory indicata da FCP.sh
cd $1
#inzio controllo
for i in *
do
	#controlliamo che sia un file e che tale file sia leggibile
	if test -f $i -a -r $i
	then
		#controlliamo il numero di righe del file 
		nl=`wc -l < $i`
		if test $nl -ne $2
		then continue #echo ERRORE_DEBUG: $i non ha $2 linee; continue;
		fi
	 	#controlliamo che tutte le sue linne contengano un carattere numerico
		N=`grep [0-9] $i | wc -l`
		if test $N -eq $nl
		then echo `pwd`/$i >> $3
		fi
	fi
done

#parte ricorsiva
for i in *
do
	if test -d $i -a -x $i
	then FCR.sh `pwd`/$i $2 $3
	fi
done

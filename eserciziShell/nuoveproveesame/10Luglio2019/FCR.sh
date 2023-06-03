#!/bin/sh

cd $1 #ci spostiamo nella directory in cui dobbiamo fare la ricerca per questa invocazione

count=0

for i in *
do
	#controlliamo che sia un file e che sia leggibile
	if test -f $i -a -r $i
	then
		#nl variabile che conta il numero di righe che hanno lettera $2
 		nl=`grep $2 $i | wc -l`
 		
 		if test $nl -ge 2
 		then
 			#incrementiamo la variabiel count che ci servirà per capire se la dyrectory analizzata soddisfa le richieste
 			count=`expr $count + 1`
 			#scrivaimo il nome di tale file sul file /tmp/creato 
 			echo `pwd`/$i >> $3
 		fi
	
	fi

done

if test $count -gt 0
then 
	echo è stata trovata una directory che soddifa le richieste: `pwd`
fi

#passiamo ora alla parte ricorsiva
 
 for i in *
 do
 	if test -d $i -a -x $i
 	then 
 		#chiamata ricorsiva cui passiamo come primo parametro il nome assoluto della directory
 		FCR.sh `pwd`/$i $2 $3
 	fi
 done
 


#!/bin/sh

cd $1 #ci spostiamo nella directory in cui dobbiamo fare la ricerca per questa invocazione

for F in *
do
	#controlliamo che sia un file e che sia leggibile
	if test -f $F -a -r $F
	then 
		#ottengo il numero di caratteri di tale file
		nc=`wc -c < $F`
		
		if test $nc -lt $2
		then 
			#il file rispetta le spefiche richieste dalla consegna
			#stampiamo il nome assouto di tale file
			echo `pwd`/$F
			
			#inseriamo il nome dei file all'interno del file temporaneo
			echo `pwd`/$F >> $3
		fi
	fi
done

 #passiamo ora alla parte ricorsiva
 
 for i in *
 do
 	if test -d $i -a -x $i
 	then 
 		#chiamata ricorsiva cui passiamo come primo parametro il nome assoluto della directory
 		FCR.sh `pwd`/$i $2 $3
 	fi
 done

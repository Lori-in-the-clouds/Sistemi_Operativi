#!/bin/sh

cd $1 #ci spostiamo nella directory in cui dobbiamo fare la ricerca per questa invocazione


for F in *
do
	#controlliamo che sia un file
	if test -f $F
	then
		#contiamo il numero di carattteri di tale file
		nc=`wc -c < $F`
		
		#controlliamo che la lunghezza di tali cratteri corrisponda con $2
		if test $nc -eq $2
		then
			#il file rispetta le specifiche
			
			#stampiamo il nome assoluto di tale file su standard output
			echo `pwd`/$F
			#inseriamo il nome assoluto di tale file all'interno del file temporaneo
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


 
 

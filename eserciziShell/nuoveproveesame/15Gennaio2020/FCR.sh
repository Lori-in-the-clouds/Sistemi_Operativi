#!/bin/sh

cd $1 #ci spostiamo nella directory in cui dobbiamo fare la ricerca per questa invocazione


for i in *
do
	#controlliamo che sia un file e sia leggible
	if test -f $i -a -r $i
	then 
		#contiamo il numero di caratteri del file
		nc=`wc -c < $i`
		
		#controlliamo che il numero di caratteri trovati sia uguale al valore H (=$2)
		if test $nc -eq $2
		then
			#inseriamo il nome assoluto del file all'interno del file temporaneo
			echo `pwd`/$i >> $3
			
			#stampiamo il nome assolut
			echo `pwd`/$i
			
		fi
	fi
done

 #passiamo ora alla parte ricorsiva
 
 for i in *
 do
 	if test -d $i -a -x $i
 	then 
 		#chiamata ricorsiva cui passiamo come primo parametro il nome assoluto della directory
 		FCR.sh `pwd`/$i $2 $3 #(inseriamo tutti i parametri)
 	fi
 done



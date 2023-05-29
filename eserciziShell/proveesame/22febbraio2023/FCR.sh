#!/bin/sh

 cd $1 #ci spostiamo nella directory in cui dobbiamo fare la ricerca per questa invocazione
 
 for F in *
 do 
	 #controlliamo che sia un file leggibile
	 if test ! -f $F -o ! -r $F
	 	then continue
	 fi 
	 
	 #parametro che mi indica la lunghezza in caratteri del contenuto di F
	 nc=`wc -c < $F`
	 if test $nc -lt $2
	 then 
	 	#inseriamo in append il nome assoluto del file nel file temporaneo
	 	echo `pwd`/$F >> $3
	 	echo File trovato: `pwd`/$F
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


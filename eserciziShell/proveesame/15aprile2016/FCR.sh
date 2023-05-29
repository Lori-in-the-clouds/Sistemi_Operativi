#!/bin/sh

 cd $1 #ci spostiamo nella directory in cui dobbiamo fare la ricerca per questa invocazione
 
 #parametro che utilizzo per vedere se è tutto a posto
 control=0
 
 for F in *
 do
 	#controlliamo che F non sia una sottodirectory
 	if test -d $F
 		then control=1; break
 	fi
 	
 	#parametro che mi indica la lunghezza in linee di F
 	nl=`wc -l < $F`
 	
 	#controlliamo che $nl sia strettamente maggiore di $X e quindi di $2
 	if test $nl -le $2
 		then control=1; break
 	fi
 done
 
 #verifichiamo che si siano rispettate le specifiche richieste
 if test $control -eq 0
 	then 
 		#inseriamo in append il direttorio trovato
		echo `pwd` >> $3 
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
 
 	

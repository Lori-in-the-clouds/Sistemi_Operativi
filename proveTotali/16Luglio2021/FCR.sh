#!/bin/sh

 cd $1 #ci spostiamo nella directory in cui dobbiamo fare la ricerca per questa invocazione
 
 LISTA= #parametro utilizzato per memorizzare i vari file che soddisfano le specifiche all'interno della directory indicata
 cont=0 #parametro utilizzato per contare il numero di file che in tale directory ripettino le specifiche da consegna
 for F in *
 do
 	#controlliamo che sia un file 
 	if test -f $F
 	then 
 		#controlliamo che il nome sia costituito esattamente da 2 cartteri
 		case $F in
 			??);; #ok
 			*) continue;;
 		esac
 		
 		#se siamo arrivati qui significa che il nome del file è di esattamente due caratteri
 		#contiamo il numero di linee di tale file
 		nl=`wc -l < $F`
 		
 		#controlliamo se la lunghezza in linee coincida con $2
 		if test $2 -eq $nl
 		then 
 			#tale file rispetta le specifiche poste da consegna
 			
 			#inseriamo il nome del file all'interno della LISTA
 			LISTA="$LISTA $F"
 			
 			#incrementiamo la variabile count
 			cont=`expr $cont + 1`
 		fi
 	fi
 done
 
 
 #controlliamo se il numero di file trovati sia maggiore uguale di 2 e minore di $3
 if test $cont -ge 2 -a $cont -lt $3
 then 
 	#stampiamo il nome assoluto di tale directory
 	echo `pwd`
 	
 	#invochiamo la parte C
 	#echo Invochiamo la parte C: $LISTA
 	main $LISTA
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

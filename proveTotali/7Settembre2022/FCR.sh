#!/bin/sh

 cd $1 #ci spostiamo nella directory in cui dobbiamo fare la ricerca per questa invocazione
 
 count=1 #parametro che utilizziamo per capire se il nostro file è in posizione dispari
 control=false
 LISTA= #lista che contine tutti i file che soddisfano le specifiche della consegna
 
 for F in *
 do
 	#controlliamo che sia un file e che sia leggibile
 	if test -f $F -a -r $F
 	then 
 		#contiamo il numero di caratteri che possiede tale file
 		nc=`wc -c < $F` 
 		
 		#controlliamo se tale valore coincida con il valore X cioè $2
 		if test $nc -eq $2
 		then 
 			#controlliamo che sia in posizione dispari
 			div=`expr $count % 2`
 			if test $div -eq 1
 			then
 				#il file si trova in posizione dispari e quindi soddisfa tutte le richieste
 				
 				#stampiamo il nome assoluto di tale file su stdout
 				echo `pwd`/$F
 				
 				#inseriamo il nome assoluto di tale file all'interno della lista
 				LISTA="$LISTA `pwd`/$F"
 				
 				#settiamo la variabile control a true
 				control=true
 				
 			fi
 		fi
 	fi
 	
 	#incrementiamo la variabile count
 	count=`expr $count + 1`
 done
 
 #nel caso in cui si sia trovato almeno un file che rispetti la specifica, invoachiamo la parte C
 if test $control = true
 then
 	#invocazione parte C
 	main $LISTA
 fi
 
 #passiamo ora alla parte ricorsiva
 for i in *
 do
 	if test -d $i -a -x $i
 	then 
 		#chiamata ricorsiva cui passiamo come primo parametro il nome assoluto della directory
 		FCR.sh `pwd`/$i $2
 	fi
 done
 
 

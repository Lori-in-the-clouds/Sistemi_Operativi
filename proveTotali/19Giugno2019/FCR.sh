#!/bin/sh

 cd $1 #ci spostiamo nella directory in cui dobbiamo fare la ricerca per questa invocazione
 count=0 #variabile utilizza per capire se sono presenti almeno due file in tale directory che ripsettano le specifiche della consegna
 LISTA= #LISTA utilizzata per memorizzare temporaneamente i nomi dei file che rispettano le specifiche della consegna
 for F in *
 do
 	#controlliamo che sia un file 
 	if test -f $F
 	then 
 		#otteniamo il numero di linee di tale file
 		nl=`wc -l < $F`
 		
 		#contrlliamo che $nl sia strettamete minore di $2
 		if test $nl -lt $2
 		then
 			#inseriamo i nomi dei file dentro la LISTA
 			LISTA="$LISTA $F"
 			#incrementiamo la variabile count
 			count=`expr $count + 1`
 		fi
 	fi
done

#contrlliamo che il numero di file trovati sia almeno uguale a 2
if test $count -ge 2
then 
	echo `pwd`
	#echo DEBUG-Invocazione parte C: $LISTA
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

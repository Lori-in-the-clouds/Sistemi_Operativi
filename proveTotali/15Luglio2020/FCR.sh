#!/bin/sh

 cd $1 #ci spostiamo nella directory in cui dobbiamo fare la ricerca per questa invocazione
 
 for F in *
 do
 	#controlliamo che sia un file
 	if test -f $F
 	then
 		#controlliamo che la lunghezza in linee di tale file sia esattamente uguale a L
 		nl=`wc -l < $F` #parametro nl contiene il numero di linee di tale file
 		
 		if test $nl -eq $2
 		then
 			#controlliamo che la lunghezza media delle sue linee sia stretttamente maggiore di 10
 			somma=0
 			count=1
 	
 			while test $count -le $nl
 			do
	 		        nc=`head -$count $F | tail -1 | wc -c`
	 		        
	 		        somma=`expr $somma + $nc`
	 		        
	 		        #incrementiamo la variabile count
	 		        count=`expr $count + 1`
	 		done
	 		
	 		#calcoliamo la media
	 		media=`expr $somma / $nl`
	 		
	 		if test $media -gt 10
	 		then 
	 			#inseriamo il nome assoluto di tale file dentro il file temporaneo
	 			echo `pwd`/$F >> $3	
	 		fi
 			
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

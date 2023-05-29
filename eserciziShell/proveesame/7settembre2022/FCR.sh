#!/bin/sh

cd $1 #ci spostiamo nella directory in cui dobbiamo fare la ricerca per questa invocazione

#parametro che utilizzo per valutare quali sono i file dispari
count=1

for F in *
do	
	tmp=`expr $count % 2`
	if test $tmp -eq 1 
	then 
		#controllo se è un file ed è leggibile
		if test ! -f $F -o ! -r $F 
			then continue
		fi
			
		 #controllo il numero di linee del file
	 	 NL=`wc -l < $F`
		 if test $NL -ne $2
		 then 
		 	continue
		 fi
		 #aggiungiamo il nome del file assoluto dentro il file temporaneo
		 echo `pwd`/$F >> $3
	 fi
	 #incrementiamo la variabile count
	 count=`expr $count + 1`
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
	 
	 
	 
	 	
	

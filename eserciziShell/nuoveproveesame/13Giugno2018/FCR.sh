#!/bin/sh

cd $1 #ci spostiamo nella directory in cui dobbiamo fare la ricerca per questa invocazione

count=0

case $1 in

*/$2)
	for i in *
	do
		#controllo che sia un file e che sia leggibile
		if test -f $i -a -r $i
		then
			#calcoliamo la lunghezza in linee
			nl=`wc -l < $i`
			
			#controlliamo che la lunghezza in linee sia uguale $3
			if test $nl -eq $3
			then
				#scriviamo il nome assoluto del file all'interno del file temporaneo
				echo `pwd`/$i >> $4
				
				#incrementiamo la variabile count
				count=`expr $count + 1`
			fi
		fi
	done;;
*) ;; #non facciamo niente
esac

if test $count -gt 0
then
	echo Abbiamo trovato la directory `pwd`
fi

 #passiamo ora alla parte ricorsiva
 
 for i in *
 do
 	if test -d $i -a -x $i
 	then 
 		#chiamata ricorsiva cui passiamo come primo parametro il nome assoluto della directory
 		FCR.sh `pwd`/$i $2 $3 $4 #(inseriamo tutti i parametri)
 	fi
 done

#!/bin/sh

cd $1 #ci spostiamo nella directory in cui dobbiamo fare la ricerca per questa invocazione
base=`basename $1`
control=false #variabile che utilizziamo per capire se esiste almeno un file leggibile all'interno delle directory che rispetta le specifiche da consegna
case $base in
	#supponiamo che si conti da 1
	#verifichiamo che il nome della directory sia composto da esattamente 3 caratteri
	$2?$2) #*/$2?$2)
		for F in *
		do
			#controlliamo che sia un file leggibile
			if test -f $F -a -r $F
			then
				#settiamo la variabile control a true
				control=true
			fi
		done
		;;
		
		
	*) ;; #non facciamo niente
esac



#controlliamo che la variaible control sia uguale a true, se è uguale a true significa che abbiamo trovato un directory che rispetta le specifiche
if test $control = true
then
	#stampiamo il nome assoluto di tale directory
	echo `pwd`
	#aggiungiamo il nome di tale directory all'interno del file temporaneo
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

 




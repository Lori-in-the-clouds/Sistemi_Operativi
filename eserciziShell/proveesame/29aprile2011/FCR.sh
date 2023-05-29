#!/bin/sh

#immettiamo nella directory indicata 
cd $1

if test $4 -eq 0
#FASE A
then 
	for i in *
	do 
		#controlliamo che sia un file 
		if test ! -f $i
			then continue
		fi
		#controlliamo se il nome di tale file coincida con $2
		if test $i = $2
			then echo `pwd` >> $3
		fi
	done

#FASE B
else 
	control=0
	for i in *
	do 
		#controlliamo che sia un file 
		if test ! -f $i
			then continue
		fi
		#controlliamo se il nome di tale file coincida con $2
		if test $i = $2
			then control=`expr $control + 1`
		fi
	done
	
	#controlliamo che si sia trovato almeno un file, se non si è trovato nessun file, si inscerisce la directory dentro il file temporaneo
	if test $control -eq 0
		then 
			echo `pwd` >> $3
			#creiamo un file vuoto e inseriamo il nome relativo semplice del file dentro tale file
			echo $i > $2
	fi

fi


#parte ricorsiva

for i in *
do 
	if test -d $i -a -x $i
		then FCR.sh `pwd`/$i $2 $3 $4
	fi
done


	

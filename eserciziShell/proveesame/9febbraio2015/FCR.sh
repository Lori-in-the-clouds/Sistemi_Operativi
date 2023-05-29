#/!bins/sh

#ci immetiamo nella directory corrente
cd $1
#variabile che mi stabilisce se ho già inserito il nome della directory dentro il file temporaneo
count=0
#variabile che mi conta il numero di file che soddifano la condizione posta
countfile=0
# echo DEBUB siamo nella directory $1
for i in *
do
	#controlliamo che $i sia un file e sia leggibile
	if test ! -f $i -o ! -r $i
		then continue # echo DEBUG-ERROR: $i non è un file oppure non è eseguibile; continue
	fi
	#contiamo le rihe del file e inseriamo tale valore nella varaibile nl
	nl=`wc -l < $i`
	#controlliamo se tale file ha esattamente $2 righe
	if test $nl -eq $2
		then
			if test $count -eq 0
			then
			#inseriamo il nome della directory all'interno del file temporaneo
			echo $1 >> $3; count=`expr $count + 1`
			#echo DEBUG valore count uguale a $count
			fi
			#incrementiamo il numero di linee
			countfile=`expr $countfile + 1`
			#inseriamo il nome del file all'interno del secondo file temporaneo
			echo $i >> $4
	fi
done

#parte ricorsiva
for i in *
do 
	if  test -d $i -a -x $i
	then FCR.sh `pwd`/$i $2 $3 $4
	fi
done

#ritorniamo il numero di file trovati
exit countfile

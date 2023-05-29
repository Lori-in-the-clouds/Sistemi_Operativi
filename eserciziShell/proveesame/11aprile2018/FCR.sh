#/!bin/sh

cd $1


#navighiamo la directory corrente
for i in *
do 
	#controlliamo che sia realmente un file e che sia eseguibile
	if test ! -f -o ! -x $i
	then continue;
	fi

	#lunghezza in linee
	nl=`wc -l < $i`

	#se il numero di righe è maggiore o uguale di $1
	if test $nl ! -ge $1
	then continue;
	fi
	#controlliamo se il numero di righe del file è minore di 5
	if test $nl -lt 5
	then
		#creiamo un file temporaneo f.NOquintae inseriamo nel nostro file temporaneo il nome del file 
		> /home/lorenzo/f.NOquinta$i
		echo /home/lorenzo/f.NOquinta$i >> $3
	else 
		#creiamo un file temporaneo f.quinta e inseriamo nel nostro file temporaneo il nome del file
		> /home/lorenzo/f.quinta$i
		echo `head -5 $i | tail -1` > /home/lorenzo/f.quinta$i

		echo /home/lorenzo/f.quinta$i >> $3
	fi
done


#parte ricorsiva
for i in *
do
	if test -d $i -a -x $i
	then
		FCR.sh $1 $2 $3
	fi
done


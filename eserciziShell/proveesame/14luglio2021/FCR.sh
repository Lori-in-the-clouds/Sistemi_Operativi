#/!bin/sh

#immettiamo nella directory indicata in partenza
cd $1
#variabile che conta i file che soddisfano le richieste
count=0
#ulteriore file temporaneo che mi permette di capire se i file giusti sono a sufficienza
> /tmp/file$$

for i in *
do
	#controlliamo che $i sia un file
	if test ! -f $i -o ! -r $i
		then continue #echo DEBUG: $i non è un file; continue;
	fi
	#controlliamo che la lunghezza dei caratteri del nome del file sia esattamente uguale a 2
	nc=`echo $i | wc -c`
	nc=`expr $nc - 1`

	if test $nc -eq 2
		then
			#controlliamo che il numero di linee del file $i sia esattamente uguale a $3 
			nl=`wc -l < $i`
			if test $nl -eq $3
				then echo `pwd`/$i >> /tmp/file$$ ; count=`expr $count + 1`
			fi

	fi
done

#controlliamo se il numero di file è a sufficienza, cioe strettamente minore di $2 ma maggiore o uguale di 2
if test $count -lt $2 -a $count -gt 1
	then 
		#abbiamo trovato una directory che soddisfa le condizioni
		echo Abbiamo trovato la directory $1 che soddifa le condizioni
		#append del file temporaneo sull'altro file temporaneo
		cat < /tmp/file$$ >> $4
fi
#eliminaimo il file temporaneo
rm /tmp/file$$

#parte ricorsiva
for i in *
do
	if test -d $i -a -x $i
		then FCR.sh `pwd`/$i $2 $2 $4
	fi
done

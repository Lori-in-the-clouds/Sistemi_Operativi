#/bin/sh

#immettiamoci nella directory indicata
cd $1


for i in *
do
	#controllo se è una directory
	if test ! -d $i -o ! -x $i
		then continue
	fi

	#contiamo il numero di caratteri del nome della directory
	nc=`echo $i | wc -c`
	nc=`expr $nc - 1`
	#controlliamo che il numero di caratteri del nome del file sia 3
	if test $nc -ne 3
		then continue
	fi

	#controlliamo che i caratteri dispari abbiano il carattere $2
	numfile=`echo $i`
	#count=1
	#control=0

	if test ! grep `$2*$2` < $numfile
		then continue
	fi

	#for j in $numfile
	#do
	#	echo DEBUG: valore $j
	#	tmp=`expr $count % 2`
	#	if test $tmp -eq 1
	#		then 
	#			if test $j -ne $2
	#			then control=`expr $control + 1`; break
	#			fi
	#	fi
	#	count=`expr $count + 1`
	#done
	#if test $control -eq 1
	#	then continue
	#fi

	#scriviamo il nome del file dentro il file temporaneo
	echo `pwd`/$i >> $3
done

#parte ricorsiva
for i in *
do
	if test -d $i -a -x $i
		then FCR.sh `pwd`/$i $2 $3
	fi
done


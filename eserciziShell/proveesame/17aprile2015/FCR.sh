#/!bin/sh

#immettiamoci nella directory corrispondente
cd $1

for i in *
do
	#controllo che il parametro passato sia un file e sia leggibile
	if test ! -f $i -o ! -r $i
		then continue
	fi

	#controlliamo che $2 linee terminino con la t
	N=`grep [*t-t] $i | wc -l`
	if test $N -ge $2
		then echo `pwd`/$i >> $3
		#echo DEBUG: `pwd`/$i
	fi

done

#parte ricorsia
for i in *
do 
	if test -d $i -a -x $i
	then
		FCR.sh `pwd`/$i $2 $3
	fi
done 

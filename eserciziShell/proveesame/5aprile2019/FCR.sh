#/!bin/sh

#posizioniamo nella directory indicata
cd $1

for i in *
do
	#controllo che sia un file e che sia leggibile
	if test ! -f $i -o ! -r $i -o ! -w $i
		then continue
	fi
	#controllo che il file abbia nome relativo semplice S.txt
	if test $i != S.txt
		then continue
	fi
	#salviamo il nome assoluto del file dentro il file temporaneo
	echo `pwd`/$i >> $2
	#salviamo la lunghezza in caratteri dentro il file temporaneo
	nc=`wc -c < $i`
	echo $nc >> $2
done

#parte ricorsiva
for i in *
do
	if test -d $i -a -x $i
		then FCR.sh `pwd`/$i $2 $3
	fi
done



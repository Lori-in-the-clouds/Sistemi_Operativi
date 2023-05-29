#/!bin/sh

#posizioniamoci nella directory indicata da FCP.sh
cd $1

for i in *
do 
	#controllo che sia un file
	if test ! -f $i
		then continue
	fi
	
	#controllo che il nome del file sia sorted 
	numfile=$i
	numfilexpected=$3
	if test  $numfile = $numfilexpected 
	then 
		#inseriamo il file F ordinato dentro sorted
		sort -f $i > sorted
		#inseriamo il nome assoluto del file dentro il file temporaneo in append
		echo `pwd`/sorted >> $2
	fi
done

#parte ricorsiva
for i in *
do 
	#controllo che $i sia una directory e che sia eseguibile
	if test -d $i -a -x $i
	then 
		#chiamata ricorsiva
		FCR.sh `pwd`/$i $2 $3
	fi
done

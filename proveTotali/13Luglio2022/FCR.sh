#!/bin/sh

cd $1 #ci spostiamo nella directory in cui dobbiamo fare la ricerca per questa invocazione

L=$2

file=$3

#eseguiamo tre volte la shift 
shift 
shift 
shift
control=true;
for F in *
do
	#controlliamo che sia un file e che sia leggibile(specifica non richiesta esplicitamente dalla connsgna ma necessaria)
	if test -f $F -a -r $F
	then 
		#ricaviamo il numero di linee di tale file
		nl=`wc -l < $F`
		#controlliamo che tale valore coincida con $L
		if test $L -eq $nl
		then
			#controlliamo che all'interno di tale file ci siano tutti i caratteri C1,C2,....CQ
			for i
			do
				 #verifichiamo che contenga il carattere passato come parametro
				 if grep $i $F > /dev/null 2>&1 #ridirigiamo standard output e standard error del grep su /dev/null
				 	then control=true #c'è il carattere
				 	
				 	else control=false; break
				 fi
			done
			
			#se ci sono tutti i caratteri, il file viene considerato
			if test $control = true
			then 
				#stampiamo il nome asosluto di tale file
				echo `pwd`/$F
				
				#inseriamo il nome asoluto di tale file sul nostro file temporaneo
				echo `pwd`/$F >> $file 
			fi
		fi
	fi
done

 #passiamo ora alla parte ricorsiva
 
 for i in *
 do
 	if test -d $i -a -x $i
 	then 
 		#chiamata ricorsiva cui passiamo come primo parametro il nome assoluto della directory
 		FCR.sh `pwd`/$i $L $file $*
 	fi
 done

#!/bin/sh
cd $1 #ci spostiamo nella directory in cui dobbiamo fare la ricerca per questa invocazione
PARI=		#lista che contiene i nomi dei file che soddisfano le condizioni e che sono in posizine pari
DISPARI=	#lista che contiene i nomi dei file che soddisfano le condizioni e che sono in posizine dispari
count=0
nc=0


index=0
for i in *
do
	#controlliamo che sia un file e che sia leggibile
	if test -f $i -a -r $i
	then
		#contiamo il numero di caratteri del file
		nc=`wc -c < $i`
		#controlliamo che il numero di caratteri del file sia uguale a K
		if test $nc -eq $3
		then
			#se si,incrementiamo la variabile count
			count=`expr $count + 1`
			control=`expr $count % 2`
			
			#inseriamolo nella lista PARI
			if test $control -eq 0 
			then
				PARI="$PARI `pwd`/$i"
			
			else 	
				DISPARI="$DISPARI `pwd`/$i"
			fi
			
		fi
		
	fi
	
	#incrementiamo la variabile index
	index=`expr $index + 1`
done
	
#controlliamo che il numero di file che soddisfano le specifiche sia uguale a H e quindi a $2
if test $count -eq $2
then
	echo Abbiamo trovato una directory che rispetta le specifiche: `pwd`
	echo Prima chiamata al file C: $PARI
	echo Seconda chiamata al file C: $DISPARI
fi

 #passiamo ora alla parte ricorsiva
 
 for i in *
 do
 	if test -d $i -a -x $i
 	then 
 		#chiamata ricorsiva cui passiamo come primo parametro il nome assoluto della directory
 		FCR.sh `pwd`/$i $2 $3 #(inseriamo tutti i parametri)
 	fi
 done






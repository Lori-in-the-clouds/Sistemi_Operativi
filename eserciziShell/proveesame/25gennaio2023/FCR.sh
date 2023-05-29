#!/bin/sh

cd $1 #ci spostiamo nella directory in cui dobbiamo fare la ricerca per questa invocazione

count1=0 #variabile che ci serve per capire quanti file ci sono con estensione $2
count2=0 #variabile che ci serve per capire quanti file ci sono con estensione $3
LIST1=   #variabile per raccogliere i file con estensione $2 
LIST2=   #variabile per raccogliere i file con estensione $3
for F in *
do 
	#controllo se F è un file
	if test ! -f $F
		then continue
	fi
	
	#controllo se F è leggibile
	if test ! -r $F
		then continue
	fi
	
	#controlliamo che abbia terminazione .E1
	case $F in 
	 	*.$2) count1=`expr $count1 + 1`
	 	      LIST1= "$LIST1 $F"
	 	      ;; 
	esac
	
	#controllo se F è anche scrivibile
	if test ! -w $F
		then continue
	fi
	
	#controlliamo che abbia terminazione .E2
	case $F in 
	 	*.$3) count2=`expr $count2 + 1`
	 	      LIST2= "$LIST2 $F"
	 	      ;; 
	esac
done

#controlliamo se il valore di cont1 e cont2 sono diversi da 0 abbiamo trovato una dir giusta
if test $count1 -ne 0 -a $count2 -ne 0
	then 
		echo Trovata directory che soddisfa le specifiche e il suo nome è: `pwd`
		for f2 in $LIST2
		do
			echo Chiamiamo la parte C con $LIST1 e $f2
		done
fi

 #passiamo ora alla parte ricorsiva
 for i in *
 do
 	if test -d $i -a -x $i
 	then 
 		#chiamata ricorsiva cui passiamo come primo parametro il nome assoluto della directory
 		FCR.sh `pwd`/$i $2 $3 $4 $5
 	fi
 done

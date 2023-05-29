#!/bin/sh

cd $1 #ci spostiamo nella directory in cui dobbiamo fare la ricerca per questa invocazione
L=$2 #variabile in cui salviamo il numero di linee dei file da cercare
TMP=$3 #variabile in cui salviamo il nome del file temporaneo
#shiftiamo 3 volte
shift
shift
shift 
for F in *
do 
	
		#controlliamo che sia un file e che sia leggibile
		if test ! -f $F -o ! -r $F
			then continue
		fi
		
		#variabile che mi indica il numero di righe
		nl=`wc -l < $F`
		
		#controlliamo se il numero di righe del file è uguale a L
		if test $L -ne $nl
			then continue
		fi
		
		#controlliamo l'occorenza di tutti i caratteri 
		control=0
		for j in $*
		do
			 #verifichiamo che contenga il carattere passato come parametro
			 if  ! grep $j $F > /dev/null 2>&1 #ridirigiamo standard output e standard error del grep su /dev/null
			 	then control=1; break
			 fi
		done
		#se il file non contiene tutti i caratteri 
		if test $control -eq 1
		then continue;
		fi
		#altrimenti,inseriamo il nome assoluto di tale file dentro il file temporaneo
		echo `pwd`/$F >> $TMP
done

 
 #passiamo ora alla parte ricorsiva
 for i in *
 do
 	if test -d $i -a -x $i
 	then 
 		#chiamata ricorsiva cui passiamo come primo parametro il nome assoluto della directory
 		$0 `pwd`/$i $L $TMP $*
 	fi
 done

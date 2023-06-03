#!/bin/sh
#FCR.sh
#file comandi ricorsivo

 cd $1 #ci spostiamo nella directory in cui dobbiamo fare la ricerca per questa invocazione
 
 L=$2
 TMP=$3
 
 #ora eseguiamo lo shift tre volte per eliminare il nome della gerarchia, la lunghezza e il nome del file temporaneo e quindi avere solo i caratteri,non serve salvare il primo parametro
 shift
 shift
 shift
 
 
 for i in *
 do
 	#controlliamo che il file sia un file e che sia leggible
 	if test -f $i -a -r $i
 	then 
 		#ora dobbiamo fare un for per tutti i caratteri perchè di deve essere una occorrenza di tutti i caratteri affinché un file sia giusto
 		trovato=true
 		for s	#per tutti i caratteri
 		do
 			if ! grep $s $i > /dev/null 2>&1
 			then 
 				trovato=false
 			fi
 		done
 		
 		if test trovato=true
 		then
 			nl=`wc -l < $i`
 			if test $nl -eq $L
 			then 
 				echo `pwd`/$i >> $TMP
 			
 			fi
 		
 		
 		
 		fi
 	fi
 done
 
 for i in *
 do
 	if test -d $i -a -x $i
 	then 
 		#chiamata ricorsiva cui passiamo come primo parametro il nome assoluto della directory
 		FCR.sh `pwd`/$i $L $TMP $* #(inseriamo tutti i parametri)
 	fi
 done

#!/bin/sh

 cd $1 #ci spostiamo nella directory in cui dobbiamo fare la ricerca per questa invocazione
 count=0 #varaibile che utilizziamo per contare i file nella gerarchia che rispettano le specifiche della consegna
 LIST= #lista utilizza per memorizzare i nomi dei file che rispettano le specifiche della consegna
 case $1 in
 	*/$2) 
 		#controlliamo che la directory abbia il nome D
 		
 		for F in *
 		do
 			#controlliamo che sia un file e che sia leggibile
 			if test -f $F -a -r $F
 			then
 				#contiamo la il numero di linee di tale file
 				nl=`wc -l < $F`
 				
 				#verifichiamo che il numero di linee sia uguale a $2
 				if test $nl -eq $3
 				then 
 					#incrementiamo la variabile count
 					count=`expr $count + 1`
 					LISTA="$LISTA $F"
 				fi
 			
 			fi
 		done
 		
 		#abbiamo terminato l'esplorazione di tale gerarchia
 		echo `pwd`
 		#stampiamo il nome assoluto di tale directory
 		
 		#stampiamo il numero di file trovati
	 	echo Numero di file trovati: $count #"Numero di file trovati:" non richiesto esplicitamente dalla consegna, inserito per chiarezza
 		
 		#supponiamo di dover invocare la parte C sono nel caso in cui si sia trovato almeno un file
 		if test $count -gt 0
 		then
	 		
	 		#DEBUG-echo Invoco parte C: $LISTA $3
	 		main $LISTA $3
 		fi;;
 	*) ;;#ok tale directory non ha il nome D
 esac
 
 #passiamo ora alla parte ricorsiva
 for i in *
 do
 	if test -d $i -a -x $i
 	then 
 		#chiamata ricorsiva cui passiamo come primo parametro il nome assoluto della directory
 		FCR.sh `pwd`/$i $2 $3
 	fi
 done

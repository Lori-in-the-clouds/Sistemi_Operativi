#!/bin/sh

#controllo il numero di parametri, controllo lasco
case $# in
	0|1|2) #errore, numero di parametri non corretto
		echo Errore: numero di parametri non corretto
		exit 1;;
	3);; #vuoto
esac

 #controllo che il parametro $1 sia un numero strettamente positivo
 case $1 in
 	*[!0-9]*) 
 		echo Errore: parametro $1 non numerico o non positivo
 		exit 2;;	
 	0)
 		echo Errore: parametro $1 uguale a 0
 		exit 3;;
 esac
 
 H=$1
 
 shift
 
 for i 
 do 
 	 #controlliamo che sia il nome assoluto di una gerarchia e quindi una directory attraversabile
	 case $i in
	 	/*)
	 		if test ! -d $i -o ! -w $i	#se nome assoluto, controlliamo sia una dir attraversabile
	 			then 
	 				echo Errore: $i non directory non attraversabile
	 				exit 4
	 		fi;;
	 	*)
	 		echo Errore: $i non nome assoluto; exit 5;;
	 esac
done

 #se arriviamo qui abbiamo finito i controlli sui parametri
 #quindi possiamo passare a settare la variabile di ambiente PATH
 
 PATH=`pwd`:$PATH
 export PATH
 

 count=0
 
 #eseguiamo le chiamate ricorsive
 for i 
 do
 	#creiamo un file temporaneo
 	> /tmp/creato-$count
 	
 	#chiamata ricorsiva
 	FCR.sh $i $H /tmp/creato-$count
 	
 	#numero di file trovati
 	nl=`wc -l` < /tmp/creato-$count
 	
 	#stampiamo il numero di file trovati
 	echo Per la directory $i sono stati trovati $nl file/files
 	if test $nl -ne 0
 	then 
	 	#controlliamo che sia pari il numero di file trovati
	 	control=`expr $nl % 2`
	 	if test $control -eq 0
	 	then 
	 		LISTA=
	 		for j in `cat /tmp/creato-$count`
	 		do
	 			LISTA="$LISTA $j"
	 		done
	 		
	 		#chiamata al file C
	 		echo Chiamata al file C: $LISTA
	 	fi
	fi
 	
 	#cancelliamo il file temporaneo
 	rm /tmp/creato-$count
 	
 	#incrementiamo la variabile count
 	count=`expr $count + 1`
 	
 done
 
 
 
 
 
 
 
 
  

#!/bin/sh

#controllo il numero di parametri, controllo (stretto/lasco)
case $# in
	0|1|2) #errore, numero di parametri non corretto
		echo Errore: numero di parametri non corretto
		exit 1;;
	*);; #vuoto
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
 
 #salviamo il valore del parametro $1 dentro la variabile X
 L=$1
 
 #eseguiamo una shift
 shift
 
 
 #controlliamo che i restanti parametri siano nomi assoluti di directory attraversabili(il fatto che siano attraversabili non è richiesto  esplicitamente dalla consegna ma è necessario)
 for i
 do
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
 
 #comportamento del programma diviso in Q fasi

 for G 
 do
 	#creiamo/azzeriamo un file temporaneo
	> /tmp/nomiAssoluti$$
 	#chiamata ricorsiva
 	FCR.sh $G $L /tmp/nomiAssoluti$$
 	
 	#contiamo il numero di file scritti nel file temporaneo
 	nl=`wc -l < /tmp/nomiAssoluti$$`
 	
 	echo Nella gerarchia $G sono stati trovati $nl file
 	
 	#chiamata al file C per ogni file trovato
 	for i in `cat /tmp/nomiAssoluti$$`
 	do
 		#echo DEBUG-invocazione parte C: $i $L
 		main $i $L
 	
 	done
 done
 
 #cancelliamo il file temporaneo
 rm /tmp/nomiAssoluti$$ 



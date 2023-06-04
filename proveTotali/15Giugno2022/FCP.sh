#!/bin/sh

#controllo il numero di parametri, controllo lasco
case $# in
	0|1|2) #errore, numero di parametri non corretto
		echo Errore: numero di parametri non corretto
		exit 1;;
	*);; #vuoto
esac

Q=$# #assegniamo alla variabile Q $#

for i 
do
	 #controlliamo che sia il nome assoluto di una gerarchia e quindi una directory attraversabile
	 case $i in
	 	/*)
	 		if test ! -d $i -o ! -w $i	#se nome assoluto, controlliamo sia una dir attraversabile
	 			then 
	 				echo Errore: $i non directory non attraversabile
	 				exit 2
	 		fi;;
	 	*)
	 		echo Errore: $i non nome assoluto; exit 3;;
	 esac

done


 #se arriviamo qui abbiamo finito i controlli sui parametri
 #quindi possiamo passare a settare la variabile di ambiente PATH
 
 PATH=`pwd`:$PATH
 export PATH
 
 
 #creo un file temporaneo
 > /tmp/creato
 
 #invocazione file ricorsivo per ognuna delle Q fasi
 
 for G
 do
 	#invocazione ricorsiva del file FCR.sh
 	FCR.sh $G /tmp/creato
 done
 
 #contiamo il numero di file trovati
 nl=`wc -l < /tmp/creato`
 
 echo Numero di file trovati: $nl
 
 #inseriamo i valori del file all'interno di una lista
 LISTA=
 for i in `cat /tmp/creato`
 do 
 	LISTA="$LISTA $i"
 done
 
 #cancelliamo il file temporaneo
 rm /tmp/creato
 
 #invocazione parte C
 #echo Invocazione parte C: $LISTA
 	main $LISTA
 
 #FINITO


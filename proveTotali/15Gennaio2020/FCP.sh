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

#salviamo il primo parametro all'interno del parametro H
H=$1

#eseguiamo una shift
shift

#controlliamo che i restanti parametri siano nomi assoluti di directory (controlliamo anche che siano attraversabili, non richiesto dalla consegna ma necessario)
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
 
 #creo un file temporaneo
 > /tmp/creato
 #eseguiamo le chiamate ricorsive
 for G 
 do
 	FCR.sh $G $H /tmp/creato
 done
 
 #contiamo il numero di file che soddisfano le richieste della consegna
 nl=`wc -l < /tmp/creato`
 
 #stampiamo su STDOUT il numero di file trovati
 echo Numero di file trovati: $nl #"Numero di file trovati" non richiesto dalla consegna, aggiunto per chiarezza
 
 #contorlliamo che il numero di file trovati sia pari
 control=`expr $nl % 2`
 
 if test $control -eq 0
 then
 	#echo Invocazione C: `cat /tmp/creato`
 	main `cat /tmp/creato`
 fi
 
 #cancelliamo il file temporaneo
 rm /tmp/creato
 
 

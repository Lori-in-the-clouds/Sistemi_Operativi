#!/bin/sh

#controllo il numero di parametri, controllo (stretto/lasco)
case $# in
	0|1|2) #errore, numero di parametri non corretto
		echo Errore: numero di parametri non corretto
		exit 1;;
	*);; #vuoto
esac

 #controllo che il parametro $1 sia un singolo carattere
 case $1 in
 	?) ;; #ok è un singolo carattere
 	*)
 		echo Errore: $1 non singolo carattere
 		exit 2;;
 esac
 
 #salviamo il parametro $1 all'interno della variabile C
 C=$1
 
 #eseguiamo una shift
 shift
 
 #controlliamo che i restanti parametri siano nomi assoluti di directory
 for i
 do
 	 #controlliamo che sia il nome assoluto di una gerarchia e quindi una directory attraversabile(non richiesto esplicitametne dalla consegna, ma neccessario)
	 case $i in
	 	/*)
	 		if test ! -d $i -o ! -w $i	#se nome assoluto, controlliamo sia una dir attraversabile
	 			then 
	 				echo Errore: $i non directory non attraversabile
	 				exit 3
	 		fi;;
	 	*)
	 		echo Errore: $i non nome assoluto; exit 4;;
	 esac
done

 #se arriviamo qui abbiamo finito i controlli sui parametri
 #quindi possiamo passare a settare la variabile di ambiente PATH
 
 PATH=`pwd`:$PATH
 export PATH
 
 count=0 #variabile che utilizziamo per creare vari file
 #creiamo un file temporaneo
 > /tmp/nomiAssoluti
 
 #parte ricorsiva organizzata in W fasi
 for G 
 do 
 	#chiamata ricorsiva
 	FCR.sh $G $C /tmp/nomiAssoluti
 done

#contiamo il nuemro di directory trovate
nl=`wc -l < /tmp/nomiAssoluti`

#stampiamo su standard output il numero di directory trovate
echo Numero di directory trovate: $nl #"Numero di directory trovate" non richiesto dalla consegna, aggiunto per chiarezza

LISTA= #varaibile che utilizzato per memorizzare temporaneamente i nomi dei file di tale gerarchia
for G in `cat /tmp/nomiAssoluti`
do
	cd $G
	#navighiamo la seguente directory
	for F in *
	do
		if test -f $F 
		then
			LISTA="$LISTA $F"
		fi
	done
	
	#invochiamo parte C
	echo DEBUG_invocazione parte C: $LISTA
	LISTA= #azzeriamo la lista
done

#cancelliamo il file temporaneo
rm /tmp/nomiAssoluti
#ho finito

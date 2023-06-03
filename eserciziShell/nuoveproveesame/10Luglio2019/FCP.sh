#!/bin/sh

#controllo il numero di parametri, controllo lasco
case $# in

	0|1|2) #errore, numero di parametri non corretto
		echo Errore: numero di parametri non corretto
		exit 1;;
	
	*);; #vuoto
esac

 #controllo che il parametro $1 sia un singolo carattere
 case $1 in
 	?) ;;
 	*)
 		echo Errore: $1 non singolo carattere
 		exit 2;;
 esac
 
 Cz=$1
 
 shift
 
 #controlliamo che sia il nome assoluto di una gerarchia e quindi una directory attraversabile
 for i in
 do
	 case $i in
	 	/*)
	 		if test ! -d $i -o ! -w $i	#se nome assoluto, controlliamo sia una dir attraversabile
	 			then 
	 				echo Errore: $1 non directory non attraversabile
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
 
 > /tmp/creato
 
 for i 
 do 
 	#chiamata al file ricorsivo
 	FCR.sh $i $Cz /tmp/creato
 done
 

 LISTA= 
 for f in `cat /tmp/creato`
 do
 	LISTA="$LISTA $f"
 done
 
 #cancelliamo il file tmp/creato
 rm /tmp/creato
 
 #chiamata al file C
 echo Chiamata al file C: $LISTA $Cx
  
  
 

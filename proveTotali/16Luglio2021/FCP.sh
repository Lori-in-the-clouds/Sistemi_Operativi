#!/bin/sh


#controllo il numero di parametri, controllo (stretto/lasco)
case $# in
	0|1|2|3) #errore, numero di parametri non corretto
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
 
 H=$1 #assegno alla variabile H il valore di $1
 
 #controllo che il parametro $2 sia un numero strettamente positivo
 case $2 in
 	*[!0-9]*)
 		echo Errore: parametro $2 non numerico o non positivo
 		exit 4;;	
 	0)
 		echo Errore: parametro $2 uguale a 0
 		exit 5;;
 esac
 
 M=$2 #assegno alla variabile M il valore di $2
 
 #eseguiamo una doppia shift
 shift
 shift
 
 #controlliamo che i restanti parametri siano nomi assoluti di directory (contorolliamo anche se siano attraversabili, anche se non richiesta dalla consegna, ma è una condizione necessaria)
 for i 
 do
  	#controlliamo che sia il nome assoluto di una gerarchia e quindi una directory attraversabile
	 case $i in
	 	/*)
	 		if test ! -d $i -o ! -w $i	#se nome assoluto, controlliamo sia una dir attraversabile
	 			then 
	 				echo Errore: $1 non directory non attraversabile
	 				exit 5
	 		fi;;
	 	*)
	 		echo Errore: $i non nome assoluto; exit 6;;
	 esac
done

 #se arriviamo qui abbiamo finito i controlli sui parametri
 #quindi possiamo passare a settare la variabile di ambiente PATH
 
 PATH=`pwd`:$PATH
 export PATH
 
 #invocazione file ricorsivo
 for G 
 do
 	#chiamata ricorsiva
 	FCR.sh $G $H $M
 done



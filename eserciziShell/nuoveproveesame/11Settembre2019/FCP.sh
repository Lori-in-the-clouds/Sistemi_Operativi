#!/bin/sh

#controllo il numero di parametri, controllo lasco
case $# in
	0|1|2|3) #errore, numero di parametri non corretto
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
 
  #controllo che il parametro $2 sia un numero strettamente positivo
 case $2 in
 	*[!0-9]*) 
 		echo Errore: parametro $1 non numerico o non positivo
 		exit 4;;	
 	0)
 		echo Errore: parametro $1 uguale a 0
 		exit 5;;
 esac
 
 #controllo che il parametro $2 sia minore di 255
 if test $2 -gt 255
 then 
 	echo Errore: parametro $2 è un numero maggiore di 255
 	exit 6;
 fi
 
 K=$2
 
 #eseguiamo una doppia shift siccome abbiamo già salvato i primi due parametri all'interno di due variabili
 shift
 shift

#se arriviamo qui abbiamo finito i controlli sui parametri
#quindi possiamo passare a settare la variabile di ambiente PATH
 
 PATH=`pwd`:$PATH
 export PATH
 


#chiamata al file ricorsivo
for i 
do 
	#chiamata ricorsiva
	FCR.sh $i $H $K
done
 

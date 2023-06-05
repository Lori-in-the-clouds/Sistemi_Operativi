#!/bin/sh

#controllo il numero di parametri, controllo (stretto/lasco)
case $# in
	0|1|2) #errore, numero di parametri non corretto
		echo Errore: numero di parametri non corretto
		exit 1;;
	*);; #vuoto
esac

count=1 #variabile che utilizzo per capire se sono arrivato al penultimo parametro
number=`expr $# - 1` 
control=false
B= #variabile che contiene l'ultimo parametro
for i
do
	if test $control = true
	then
		B=$i; break
	fi
	
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

	#controlliamo se siamo arrivati al penultimo parametro
	if test $count -eq $number
	then
		control=true #se siamo arrivati al penultimo parametro,saltiamo
	fi

	#incremento la variabile count
	count=`expr $count + 1`
done

 #se arriviamo qui abbiamo finito i controlli sui parametri
 #quindi possiamo passare a settare la variabile di ambiente PATH
 
 PATH=`pwd`:$PATH
 export PATH
 
 count=1
 for G 
 do
 	#chiamata ricorsiva
 	FCR.sh $G $B
 	
 	if test $count -eq $number
 	then
 		break;
 	fi
 	
 	#incremento la variabile count
	count=`expr $count + 1`
done



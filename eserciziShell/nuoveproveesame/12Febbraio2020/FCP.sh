#!/bin/sh

#controllo il numero di parametri, controllo lasco
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
 
 #controllo che $1 sia dispari
 nl=`expr $1 % 2`
 
 if test $nl -ne 1
 then 
 	echo Errore: parametro $1 non è un numero dispari
 	exit 4
 fi
 
 #inseriamo $1 nella varaibile K1
 K1=$1
 
 
 #controllo che il parametro $2 sia un numero strettamente positivo
 case $2 in
 	*[!0-9]*) 
 		echo Errore: parametro $2 non numerico o non positivo
 		exit 4;;	
 	0)
 		echo Errore: parametro $2 uguale a 0
 		exit 5;;
 esac
 
 #controllo che $2 sia pari
 nl=`expr $2 % 2`
 
 if test $nl -ne 0
 then 
 	echo Errore: parametro $2 non è un numero pari
 	exit 6
 fi
 
 #inseriamo $2 nella varaibile K2
 K2=$2
 
 #eseguiamo due volte la shift
 shift 
 shift
 
 #controlliamo che i restanti parametri siano nomi assoluti di directory attraversabili
 for i 
 do
 	case $i in
 	/*)
 		if test ! -d $i -o ! -w $i	#se nome assoluto, controlliamo sia una dir attraversabile
 			then 
 				echo Errore: $i non directory non attraversabile
 				exit 7
 		fi;;
 	*)
 		echo Errore: $i non nome assoluto; exit 8;;
 	esac
done

#se arriviamo qui abbiamo finito i controlli sui parametri
#quindi possiamo passare a settare la variabile di ambiente PATH
 
 PATH=`pwd`:$PATH
 export PATH
 
 #creiamo un file temporaneo
 > /tmp/creato
 

#chiamata al file ricorsivo
for i
do 	
	#chiamata al file ricorsivo
	FCR.sh $i $K1 $K2 /tmp/creato

done

LINEA=
for i in `cat /tmp/creato`
do
	LINEA="$LINEA $i"
done

#chiamata al file C
echo Chiamata al file C: $LINEA

#elimino il file temporaneo
rm /tmp/creato
 
 
 
 



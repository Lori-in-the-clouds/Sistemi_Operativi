#!/bin/sh

#controllo il numero di parametri, controllo lasco
case $# in
	0 | 1  | 2)
			#errore, numero di parametri non corretto
			echo Errore: numero di parametri non corretto
			exit 1;;

	*) ;; #ok
esac
#parametro che utilizzo per non valutare l'ultimo parametro
count=1
X=0
#controllo sui restanti parametri 
for i in $*
do 
	if test $count -ne $#
	then 
		 case $i in
	 	/*)
	 		if test ! -d $i -o ! -w $i	#se nome assoluto, contrlliamo sia una dir attraversabile
	 			then 
	 				echo Errore: $i non directory non attraversabile
	 				exit 2
	 		fi;;
	 	*)
	 		echo Errore: $i non nome assoluto; exit 3;;
 		esac
 	else 
 		 #controllo che l'ultimo parametro sia un numero strettamente positivo
		 case $i in
		 	*[!0-9]*) 
		 		echo Errore: parametro $i non numerico o non positivo
		 		exit 4;;	
		 	0)
		 		echo Errore: parametro $i uguale a 0
		 		exit 5;;
		 esac
		 
		 #inseriamo nella variabile X l'ultimo parametro
		 X=$i
		
	
	fi
	
	#incrementiamo la variabile count
	count=`expr $count + 1`
done

#se arriviamo qui abbiamo finito i controlli sui parametri
#quindi possiamo passare a settare la variabile di ambiente PATH
 
 PATH=`pwd`:$PATH
 export PATH
 
 count=1
 
 #creiamo un file temporaneo 
 > /tmp/count$$
 
 for G in $*
 do
 	if test $count -ne $#
	then 
 		#chiamata ricorsiva
 		FCR.sh $G $X /tmp/count$$
 	fi
 	
 	#incrementiamo la variabile count
	count=`expr $count + 1`
 done
 
 #parametro che mi indica il numero di direttori trovati
 nl=`wc -l < /tmp/count$$`
 #stampiamo il numero di direttori trovati		
 echo Numero totale di directory trovate : $nl
 
 for i in `cat /tmp/count$$`
 do
 	echo Directory trovata: $i
 	#inseriamoci nella directory trovata
 	cd $i
 	for j in *
 	do 
 		echo file della directory $i:`pwd`/$j
 		#stampiamo la X- esima riga del file partendo dal basso
 		tail -$X $j | head -1
 	done
 done
 #cancelliamo il file temporaneo
 rm /tmp/count$$
 
 echo ho finito


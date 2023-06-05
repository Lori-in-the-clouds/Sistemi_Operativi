#!/bin/sh

cd $1 #ci spostiamo nella directory in cui dobbiamo fare la ricerca per questa invocazione

#G=`basename $1`
G=$1
control=false	#variabile control che utilizzo per capire se il file èall'interno di tale directory
#eseguiamo una shift
shift 

case $G in
	*/*[0-9]*[^/]) #il nome della directory contiene almeno un numero
	
		#controlliamo che tale directory contenga tutti i file F1,....FQ
		for i
		do
			control=false	#settiamo la variabile control a false, all'inzio non l'abiamo ancora trovato il file
			
			for F in *
			do
				if test -f $F
				then
					#se il nome dei file sono uguali
					
					if test $F = $i
					then
						control=true
					fi
				
				fi
			done
			
			if test $control = false
			then
				break;
			fi
		done
		
		#abbiamo trovato tutti i file dentro tale directory
		if test $control = true
		then
			echo `pwd`
			#echo Invoco parte C: $*
			main $*
		fi ;;
	
	*);;	#non facciamo niente
esac

 #passiamo ora alla parte ricorsiva
 for i in *
 do
 	if test -d $i -a -x $i
 	then 
 		#chiamata ricorsiva cui passiamo come primo parametro il nome assoluto della directory
 		FCR.sh `pwd`/$i $*
 	fi
 done

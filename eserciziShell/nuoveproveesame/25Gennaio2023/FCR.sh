#!/bin/sh
#FCR.sh

E1=$2
E2=$3
LISTA1=		#lista utilizzata per salvare i nomi dei file avente estensione .E1
LISTA2=		#lista utilizzata per salvare i nomi dei file avente estensione .E2
cont1=0
cont2=0

cd $1 #ci spostiamo nella directory in cui dobbiamo fare la ricerca per questa invocazione

controllo=false

for i in *
do

	#cerchiamo di trovare il file con estensione .E1
	if test -f $i -a -r $i
	then 
		case $i in 
 			*.$E1) 
 				#file con estensione .E1 trovato,salviamolo all'interno della lista1
 				LISTA1="$LISTA1 $i"
 				cont1=`expr $cont1 + 1`
 				;;
 		esac
 	fi
 		
 	#cerchiamo di trovare il file con estensione .E2
 	if test -f $i -a -w $i
 	then 
 		case $i in 
 			*.$E2) 
 				#file con estensione .E2 trovato,salviamolo all'interno della lista2
 				LISTA2="$LISTA2 $i"
 				cont2=`expr $cont2 + 1`
 				;;
 		esac	
	fi
done

#contorllo che cont1 e cont2 siano diversi da 0
if test $cont1 -ne 0 -a $cont2 -ne 0
then 
	echo Trovata directory che soddisfa la condizione e il suo nome assoluto è: \'`pwd`
	
	for f2 in $LISTA2
	do
		echo chiamiamo la parte C con $LISTA1 $f2
		#main $f2 $LISTA1
	
	done
fi

 #passiamo ora alla parte ricorsiva
 
 for i in *
 do
 	if test -d $i -a -x $i
 	then 
 		#chiamata ricorsiva cui passiamo come primo parametro il nome assoluto della directory
 		FCR.sh `pwd`/$i $1 $2 #(inseriamo tutti i parametri)
 	fi
 done
 


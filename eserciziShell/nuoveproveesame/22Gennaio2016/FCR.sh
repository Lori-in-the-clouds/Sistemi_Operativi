#!/bin/sh

 cd $1 #ci spostiamo nella directory in cui dobbiamo fare la ricerca per questa invocazione
 
 LISTA=	#Vvaraibile che conterrà i nomi assoluti dei file del direttorio che rispetta le specifiche
 
 control=true
 count=0
 for i in *
 do
 	if test -f $i -a -r $i
 	then 
 		#incrementiamo la variabile count
 		count=`expr $count + 1`
 		
 		#aggiungiamo il nome del file dentro la lista
 		LISTA="$LISTA `pwd`/$i"
 		
 	fi
 	
 	if test -d $i 
 	then
 		control=false; break
 	fi
 
done

dueN=`expr $2 \* 2`

if test $count -eq $dueN -a $control = true
then 
	echo Direttorio trovato: `pwd`
	#invocazione parte C
	echo Invocazine parte C: $LISTA
fi

 #passiamo ora alla parte ricorsiva
 
 for i in *
 do
 	if test -d $i -a -x $i
 	then 
 		#chiamata ricorsiva cui passiamo come primo parametro il nome assoluto della directory
 		FCR.sh `pwd`/$i $2 
 	fi
 done

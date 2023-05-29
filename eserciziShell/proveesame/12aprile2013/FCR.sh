#!/bin/sh

#posizioniamoci nella directory corrente
cd $1
control=0
for i in *
do 
	#controlliamo che sia un file leggibile
	if test ! -f $i -o ! -r $i
		then continue
	fi
	
	case $i in
	*.$2) 
		#solo se è la prima volta
		if test $control -eq 0
			then echo `pwd` >> $3
		fi
		
		#incrementiamo la variabile control
		control=`expr $control + 1`;; #ok
	esac
done


#parte ricorsiva
for i in *
do 
	if test -d $i -a -x $i
		then FCR.sh `pwd`/$i $2 $3
	fi
done


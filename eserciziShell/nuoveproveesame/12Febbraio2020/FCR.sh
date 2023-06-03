#!/bin/sh
 
cd $1 #ci spostiamo nella directory in cui dobbiamo fare la ricerca per questa invocazione

count_pari=0	#parametro utilizzato per contare il numero di file che hanno numero di caratteri uguali a K1
count_dispari=0	#parametro utilizzato per contare il numero di file che hanno numero di caratteri uguali a K2
file1=
file2=

for i in *
do
	#controlliamo che sia un file e che sia leggibile
	if test -f $i -a -r $i
	then
		#contiamo il numero di caratteri del file
		nc=`wc -c < $i`
		
		#controlliamo se il numero di caratteri del file corrisponde a K1
		if test $nc -eq $2   
		then
			
			file1=`pwd`/$i
			count_dispari=`expr $count_dispari + 1`
		fi
		
		#controlliamo se il numero di caratteri del file corrisponde a K2
		if test $nc -eq $3  
		then
			
			file2=`pwd`/$i
			count_pari=`expr $count_pari + 1`
		fi
	fi
done
	
#controlliamo se la directory rispetta le specifiche, cioè contiene un solo file avente lunghezza uguale a K1 e un solo file avante lunghezze uguale a K2
if test $count_dispari -eq 1 -a $count_pari -eq 1
then
	#stampiamo il nome assoluto di tale directory 
	echo Directory che rispetta le specifiche: `pwd`
	
	#in questo caso salviamo i nomi dei due file all'interno del file temporaneo
	echo file2 >> $4
	echo file1 >> $4
	
fi

 #passiamo ora alla parte ricorsiva
 
 for i in *
 do
 	if test -d $i -a -x $i
 	then 
 		#chiamata ricorsiva cui passiamo come primo parametro il nome assoluto della directory
 		FCR.sh `pwd`/$i $2 $3 $4
 	fi
 done





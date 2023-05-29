#!/bin/sh

#immettiamoci nella directory indicata
cd $1

#incrementiamo il count
count=`expr $2 + 1`

for i in *
do
	if test -d $i -a -x $i
		then 
			FCR.sh `pwd`/$i $count $3 $4
			ret=$?
			
			#leggiamo l'ultimo record
			read prec < /tmp/tmpContaLivelli
			
			if test $ret -gt $prec
				then 
					echo $ret > /tmp/tmpContaLivelli
			fi
	fi
done

if test $3 = B
	then 
		if test $count -eq $4
			then echo Adesso visuallizzo il contenuto della directory `pwd` che fa parte del livello $4:
				ls -la `pwd`
		fi
fi

exit $count

#/!bin/sh

#immettiamo nella directory indicata dal parametro $1
cd $1

for i in *
do
	#controlliamo che sia un file
	if test ! -f $i
		then continue
	fi
	F=$i
	#controliamo che il nome del file finsca con .S
	control=0
	case $F in
		*.$3) echo `pwd`>> $4; control=`expr $control + 1`;;
	esac

	if test $control -ne 0
	then break;
	fi
done

#parte ricorsiva
for i in *
do 
	if test -d $i -a -x $i
	then
		FCR.sh `pwd`/$i $2 $3 $4
	fi
done


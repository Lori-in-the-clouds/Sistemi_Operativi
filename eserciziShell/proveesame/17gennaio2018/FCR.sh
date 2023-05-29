#/!bin/sh
#rinominiamo tutti le variabili 
G=$1 #directory
N=$2 #numero di file
H=$3 #numero di righe
Cx=$4 #carattere

cd $G #ci spostiamo nella directory in cui dobbiamo fare la ricerca per questa invocazione

#parametro di conteggio del numero di file all'intendo della directory
contfile=0
#parametro dove inserisco tutti i file ottenuti
LIST=

for i in *
do
#verifico se è un file, e se lo è incremento il contfile
if test -f $i
	then
	#verifico il numero di righe
	nl=`wc -l < $i`
		if test $nl -ne $H
		then continue #echo DEBUG-ERROR: $i non ha $H righe;
		fi
	#verifico se c'è Cx
	if ! grep $Cx $i > /dev/null 2>&1
	then continue #echo DEBUG-ERROR: $i non ha il carattere $Cx;
	fi
	#incrementiamo il contatore
	contfile=`expr $contfile + 1`; LIST="$LIST `pwd`/$i "
fi
done
#controlliamo che il numero di file sia uguale a N
if test $contfile -ne $N
	then exit 4 #echo DEBUG-ERROR: non ci sono $N file che soddifano le condizioni poste; 
	else echo Trovata directory che soddisfa le condizioni: `pwd`
fi
#ricorsione
for i in *
do
	if test -d $i -a -x $i
	then FCR.sh `pwd`/$i $N $H $Cx
	fi
done

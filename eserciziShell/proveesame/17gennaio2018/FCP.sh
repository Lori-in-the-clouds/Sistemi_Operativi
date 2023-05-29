#!/bin/sh
#eseguiamo i controllo stretto dei parametri, verifichiamo che siano passati 4 parametri
case $# in
4) ;; #ok
*) echo Errore: numero di parametri passati errato; exit 1;;
esac
G=$1 #deve essere una directory eseguibile
N=$2 #deve essere numero intero
H=$3 #deve essere numero intero
Cx=$4 #deve essre un singolo carattere
#controlliamo che il primo parametro sia una directory e che sia eseguibile 
case $G in
/*) if test ! -d $G -a ! -x $G
    then Errore: $G non è una directory oppure non è eseguibile; exit 2
    fi;;
*)echo  Errore: $G non è un nome assoluto; exit 3;;
esac
#controllo che il secondo e il terzo parametro siano maggiori di 0
if test $N -lt  0 -a $H -lt  0
then echo Errore: $N o $H non sono numeri interi; exit 4
fi
#controllo che il quarto parametro sia un solo carattere
case $Cx in
?);; #ok
*)echo Errore: $Cx non è un parametro composto da u solo carattere; exit 5;;
esac
#settiamo il path 
PATH=`pwd`:$PATH
export PATH

#invocazione del file ricorsivo
FCR.sh $*

echo Qui abbiamo già finito

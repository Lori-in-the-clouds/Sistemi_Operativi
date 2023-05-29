#!/bin/sh

#facciamo un cotrollo stretto sui parametri
if test $# -ne 2
	then echo Errore: numero di parametri passato non corretto
fi

#controlliamo che il primo parametro sia un nome in forma assoluta
case $1 in
	/*)
		#controlliamo che sia una directory e che sia eseguibile
		if test ! -d $1 -o ! -w
		then echo Errore: parametro $1 non è una directory oppure tale directory non è eseguibile; exit 1
		fi;;
	*) echo Errore: parametro $1 non in forma assoluta; exit 2;;
esac

#controlliamo che il secondo parametro sia un numero strettamente positivo
if test $2 -lt 0
	then echo Errore: parametro $2 non strettamente positivo; exit 3
fi

#tutti i controlli sono stati effettuati, settiamo la variabile path
PATH=`pwd`:$PATH
export PATH

#andiamo a creare un file /tmp/conta$$ il cui nome varrà passato come ulteriore argomento al file comandi FCR.sh
> /tmp/conta$$

#chiamata al file ricorsivo 
FCR.sh $1 $2 /tmp/conta$$

#stampiamo i nomi dei file trovati 
echo File trovati:`cat /tmp/conta$$`
echo Ho finito tutto

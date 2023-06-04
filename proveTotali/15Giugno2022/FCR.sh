
 cd $1 #ci spostiamo nella directory in cui dobbiamo fare la ricerca per questa invocazione
 
 control=false #varaibile control che utilizziamo per capire se esiste almeno un file in tale directory che soddisfa le specifiche richieste
 
 for F in *
 do
 	#controlliamo che sia un file e che sia leggibile (la leggibilità non è una richiesta esplicitamente della consegna, ma necessaria)
 	if test -f $F -a -r $F
 	then
 		#controlliamo che il file non sia nullo
 		NC=`wc -c < $F`
 		if test $NC -eq 0
 		then
 			continue #nel caso in cui il file sia nullo
 		fi
 		
 		#controlliamo che nel file ci siano solamente lettere minuscole
 		 ND=`grep -c [^a-z] $F`
		 if test $ND -eq 0
		 then 
		 	#ok in questo file ci sono solo lettere minuscole
		 	control=true
		 	#stampiamo il nome del file
		 	echo File trovato: `pwd`/$F
		 	#aggiungiamo il nome del file all'iterno del nostro file temporaneo (gli riportiamo in forma assoluta, non esplicitamente richiesto dal testo)
		 	echo `pwd`/$F >> $2
		 fi
 	
 	fi
done


 #passiamo ora alla parte ricorsiva
 for G in *
 do
 	if test -d $G -a -x $G
 	then 
 		#chiamata ricorsiva cui passiamo come primo parametro il nome assoluto della directory
 		FCR.sh `pwd`/$G $2
 	fi
 done



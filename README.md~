# Patisserie

Programma per il corso di Algoritmi e principi dell'informatica del Politecnico di Milano (as. 2023-2024).
Il codice ha ricevuto una valutazione di 30L/30 completando il test con:
    
    1.198 secondi su 1.5 minimi richiesti
    9,60 MiB su 14 minimi richiesti

## Bug 

Sia nel file main sia nel file finale (quello da consegnare) è presente una flag BUG. Questo perchè mi sono reso conto che nonstante sul sito mi desse output corretto c'era qualche testcase che mi dava errori. Ho scoperto esserci un bug con un if. Mi ero detto che un giorno l'averei fixato ma eccoci qui, il bug ancora c'è e volendo si può riattivare.

## Perchè ho condiviso il codice

Inanzitutto perchè i file:
    
    src/algortmi.c 
    src/bst.c 
    src/rbt.c 

Sono la copia già pronta all'uso degli pseudocodici utilizzati dai professori nelle loro slide. 

Inoltre perchè i file .sh sono file che possono aiutare chi non ha molta dimestichezza con il terminale a velocizzare di molto il processo di building e testing della loro applicazione. Invito vivamente chi sta leggendo a dargli un'occhiata perchè sono molto sempici da implementare e vale la pena perdere qualche minuto a capirli per poi risparmiare molto tempo in futuro (sopratutto se ci si trova un po' all'ultimo nello sviluppare il codice).

Il file 

    src/main.c 

dove avviene tutta la magia, ho cercato di scriverlo il più leggibile possibile. E' più leggibile il codice dei miei commenti probabilmente. L'ho fatto anche perchè dando una letta al codice possiate vedere come ho deciso di dividere le varie task e affontare i vari problemi in modo facile e veloce. 

Capisco che molti non avranno comunque molta voglia di leggere 900 linee di codice, ci sta anche io non avrei voglia.

Buona fortuna per il vostro progetto!

## Come buildare e lanciare il codice

Parto dal pesupposto che si stia usando linux.

Ho usato un file bash per il building dell'eseguibile

Usa:
    
    chmod +x build.sh

per rendere eseguibile il file.

Per buildare il codice in modalità release:

    ./build.sh file main 0

Per buildare il codice in modalità debug:

    ./build.sh file main 1 

Per lanciare il codice:
    
    ./build/main

Per generare i file valgrind massif dei test più interessanti:

    ./build.sh massif main

Per generare i file valgrind callgrind dei test più interessanti:

    ./build.sh callgrind main

Se si vogliono testare altri casi è facilmente scalabile il codice del file build.sh

## Come testare il codice

Sempre con l'utilizzo di un file bash.

Usa:

    chmod +x test.sh

per rendere eseguibile il file.

Se si vuole stampare il confronto completo:
    
    ./test.sh test_out main

Se invece si è interessati a testare solo le differenze si può usare:

    ./test.sh test_diff main
    

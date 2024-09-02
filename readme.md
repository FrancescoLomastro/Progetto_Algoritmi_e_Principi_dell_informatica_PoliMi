## Progetto di algoritmi e strutture dati
Qeesta è la repository che comprende il progetto del corso di Algoritmi e Principi dell'informatica (algoritmi e strutture dati), anno 2021/22 del Politecnico di Milano.

## Struttura della repository
- Testo progetto: PDF in cui è presentata la traccia del progetto.
- Direttive generali: PDF in cui sono mostrate le regole della prova e eventuali contatti dei tutor.
- main.c: File .C contenente il codice del progetto.
- Tests: Cartella contenente alcuni file di test utilizzati durante lo sviluppo.
- readme.md e readme_images: FIle di intestazione della repository.



## Deployment
Il codice può essere testato con un qualsiasi IDE per file.c, bisogna configurare l'IDE in modo che fornisca uno dei file nella cartella Tests al posto del file stdin.
Alternativamente può essere usato anche il file stdin ma il progetto è ideato per considerare grandi quantità di parole.


# Idea alla base
Il progetto richiede di memorizzare grandi quantità di parole (anche diverse centinaia di migliaia) e soprattutto di ordinarle in ordine lessicografico.
L'idea iniziale è stata quella di creare un albero rosso nero (in modo da avere una struttura bilanciata) e assegnare ad ogni parola un flag che ne indicasse la compatibilità con i vincoli appresi dai tentativi. <br>
A seguito di alcune prove sui test disponibili ho deciso di attuare alcune ottimizzazioni

## Ottimizzazione 1
Dato che possono esserci diverse partite (mantenendo le stesse parole) è necessario che, tra una partita e l'altra, si resettino tutti i flag di validità <br>
In questo modo una nuova partita può iniziare considerando tutto l'elenco di parole da usare. <br>
Per resettare questi flag bisognerebbe scorrere la struttura dati, soffermarsi su ogni parola memorizzata e ripristinare il relativo flag. <br>
Questo compito dispendioso può essere evitato cambiando la struttura dei flag: <br>

Utilizzo 2 puntatori globali (chiamati flag) Ultimo_n (punta ad una 'n', cioè Parola non compatibile) e Ultimo_y (punta ad una 'y', cioe Parola compatibile).
I nodi contententi le parole hanno un puntatore ad uno di questi 2 flag a seconda che la parola del nodo sia compatibile o no.
Al momento del reset, mi basta cambiare la lettera puntata da Ultimo_n in con una 'y', ed ottengo un reset istantaneo di tutte le parole.
Dopodichè Ultimo_y deve puntare a questa lettera modificata, e Ultimo_n punterà ad una nuova lettera 'n'.

<p align="center">
  <img src="readme_images/ottimizzazione_1_1.png" alt="alt text" width="400"/>
<img src="readme_images/ottimizzazione_1_2.png" alt="alt text" width="400"/>
</p>

## Ottimizzazione 2
L'enorme quantità di parole memorizzate rende lenta l'esplorazione e il filtraggio. 
Per velocizzare il processo decido di creare piu alberi al posto di un unico albero e inoltre introdurre dei flag (uguali a quelli per parola) ma che indichino la validità di interi alberi.
Creo una matrice che indicizzi quale albero usare, data la prima e la seconda lettera, dunque una matrice 64x64.

<p align="center">
<img src="readme_images/ottimizzazione_2_1.png" alt="alt text" width="600"/>
</p>

Con questa matrice di alberi la ricerca e il filtraggio risulta molto piu efficiente quando si hanno gia alcune informazioni sulla parola di riferimento.



## Ottimizzazione 3
Con l'ottimizzazione 2 tutti gli alberi risultano molto meno alti rispetto a prima, diventa quindi poco utile mantenere la caratteristica rosso-nera, la quale si preoccupava di mantenere bilanciato l'unico albero che avevamo.
L'ottimizzazione 3 consiste nel rimuovere questa caratteristica e tornare a delle normali e (potenzialmente sbilanciate) strutture ad albero tradizionale. 


## Valutazione
Il seguente progetto ha conseguito una valutazione di 30 e lode.
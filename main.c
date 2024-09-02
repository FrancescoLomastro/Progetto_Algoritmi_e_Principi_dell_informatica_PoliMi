#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUMERO_CARATTERI 64

/** Nodo di un albero che contiene le stringhe **/
struct Nodo {
    char *key;                  /** Punta alla stringa del Nodo **/
    struct Nodo *left;          /** Punta al figlio sinistro del nodo (se c'è) **/
    struct Nodo *right;         /** Punta al figlio destro del nodo (se c'è) **/
    char* valido;               /** Punta ad un char che definisce se la stringa del nodo è compatibile o no **/
};

/** Albero **/
typedef struct {
    struct Nodo *radice;
    char* valido;               /** Punta ad un char che definisce se l'intero albero (descritto dalle prime 2 lettere) è compatibile o no **/
}Albero;


typedef struct
{
    char presente;              /** Indica se il carattere è presente o no nella parola da indovinare **/
    short numero_minimo;        /** Indica il numero minimo di occorrenze del carattere nella parola da indovinare **/
    char **posizioni_vietate;   /** Si tratta di un vettore di puntatori, ogni cella indica una posizione nella parola riferimento, in ogni cella si trova un puntatore ad ultimo_n o ultimo_y. In qeusto modo indentifico le posizioni in cui il carattere è vietato*/
    char flag_massimo;          /** Indica che numero massimo (= minimo) di occorrenze del carattere nella parola da indovinare è stato raggiunto ed è pari a numero_minimo **/

    // Sotto questa riga si trovano delle variabili che vengono resettate ad ogni tentativo, quindi sono ausiliarie per l'analisi di ogni parola **/
    short counter;              /** Conta quante occorrente di un certo carattere sono presenti nella parola ternativo attuale **/
    char flag_massimo_counter;  /** Indica che il counter per la parola corrente ha raggiunte il numeoro massimo di occorrenze **/
}Vincoli_carattere;


/** Strutttura di controllo ausiliaria, viene usata quando si analizza una parola tentativo. Per ogni carattere conta
 * quante occorrenze in posizione corretta e scorretta sono state trovate**/
typedef struct
{
     short numero_posizione_corretta;       /
     short numero_posizione_scorretta;
}Contatore;


int lunghezza_parole;
short lunghezza_parole_1;
short lunghezza_input;
char* parola_riferimento;
char* parola_tentativo;
char* parola_dedotta;
char* parola_simboli;

/** Questi 2 puntatori tengono traccia degli ultimi flag 'n' e 'y'
 * Quando si gioca una nuova partita, ultimo_n cambia la sua lettera in 'y' e viene puntato da ultimo_y, ripristinando tutti i
 * nodi che non erano compatibili, viene poi creato un nuovo flag per ospitare la lettera 'n' ovvero il nuovo ultimo_n
 * in questo modo la pulizia dell'albero è quasi immediata, non ho bisogno di scorrere la struttura cambiando il
 * "compatibile" ad ogni nodo **/
char* ultimo_n;     /** Flag 'n' piu recente **/
char* ultimo_y;     /** Flag 'y' piu recente **/

Albero matrice_alberi[NUMERO_CARATTERI][NUMERO_CARATTERI];
Vincoli_carattere vettore_per_vincoli[NUMERO_CARATTERI];
struct Nodo* nil;
short vettore_occorrenze_riferimento[NUMERO_CARATTERI];
Contatore vettore_per_risultato[NUMERO_CARATTERI];
int var;

char dizionario_iniziale();
void inserimento_nell_albero(Albero *T, struct Nodo *z);
int fromAsciiToIndex(int ascii);
int confronta_stringhe(char *key, char *key1);
void inserisci_inizio();
void nuova_partita();
int cerca_parola_tentativo();
void calcola_vincoli();
void filtra();
void filtra_albero(struct Nodo *x);
short compatibile(char *aprola);
void inserisci_inizio_filtrato();
void stampa_filtrate();
void stampa_albero(struct Nodo *nodo);


int main()
{
    char* decisione;                                    /** conterrà la decisione "+nuova_partita" o "+inserisci_inizio" **/
    char ***spazio_posizioni;

    ultimo_n = malloc(1);                           // Inizializza i flag
    ultimo_y = malloc(1);                           //
    *ultimo_n='n';                                      //
    *ultimo_y='y';                                      //

    nil = malloc(sizeof (struct Nodo));             /** Inizializza il nodo NIL globale **/

    var = scanf("%d",&lunghezza_parole);          /** Input lunghezza parole **/

    lunghezza_parole_1 = lunghezza_parole+1;

    /** L' utente potrebbe inserire una parola di lunghezza lunghezza_parole oppure un comando che può avere intorno a 20 caratteri **/
    if(lunghezza_parole < 20)
        lunghezza_input = 20;
    else
        lunghezza_input = lunghezza_parole_1;


    spazio_posizioni = malloc(NUMERO_CARATTERI * sizeof(char **));     /** Crea la matrice di char (parte 1) **/
    for(int i=0; i<NUMERO_CARATTERI; i++) {
        spazio_posizioni[i] = malloc(lunghezza_parole * sizeof(char*));

        for (var = 0; var < NUMERO_CARATTERI; var++) {
            matrice_alberi[i][var].valido = ultimo_y;    /** Tutti gli alberi sono inizialmente validi **/
            matrice_alberi[i][var].radice = nil;         /** L'albero inizialmente non esiste **/
        }


        vettore_per_vincoli[i].posizioni_vietate = spazio_posizioni[i];
        vettore_per_risultato[i].numero_posizione_corretta=0;
        vettore_per_risultato[i].numero_posizione_scorretta=0;
        for(var=0;var<lunghezza_parole;var++)
            vettore_per_vincoli[i].posizioni_vietate[var]=ultimo_y;
    }

    /** Inizializzazione stringhe con lunghezza_parola **/
    parola_tentativo    = malloc(lunghezza_parole_1);
    parola_riferimento  = malloc(lunghezza_parole_1);
    parola_dedotta      = malloc(lunghezza_parole_1);
    parola_simboli      = malloc(lunghezza_parole_1);


    if(dizionario_iniziale()=='n')          //Prende in input le parole del dizionario e restituisce la seconda lettera del comando inserito dall'utente dopo il dizionario
        nuova_partita();
    else
        inserisci_inizio();                 //Il dizionario potrebbe essere vuoto, infatti si può iniziare anche dal +inserisci_inizio

    // Il resto delle interazioni è gestito da un ciclo
    decisione = malloc(lunghezza_input);
    var = scanf("%s",decisione);

    while(var != -1) {
        if (decisione[1] == 'n')
            nuova_partita();
        else
            inserisci_inizio();
        var = scanf("%s", decisione);
    }
    return 0;
}

/** Controlla se la parola passata come parametro rispetta i vincoli (return 1) oppure no (return 0) **/
short compatibile(char *parola) {
    short i;
    char vettore[NUMERO_CARATTERI] = {0}; /** Per ogni carattere ho un contatore di occorrenze **/
    short indice;

   /** La parola deve avere gli stessi caratteri di parola_dedotta (se ne ha) **/
    for(i=0; i<lunghezza_parole; i++) {
        indice = fromAsciiToIndex(parola[i]);
        if((parola_dedotta[i] != '.') && (parola_dedotta[i] != parola[i]))
            return 0;
        vettore[indice]++;               // Ne approfitto per contare quante occorrenze di caratteri ha la parola
    }
    /** per tutti gli altri caratteri controllo che si rispetti  il vincolo di presenza, di posizione e di massimo **/
    for(i=0; i<lunghezza_parole; i++) {
        indice = fromAsciiToIndex(parola[i]);
        if(parola_dedotta[i] == '.') {
            if(vettore_per_vincoli[indice].presente == 'n')
                return 0;
            if(vettore_per_vincoli[indice].posizioni_vietate[i] == ultimo_n)
                return 0;
            if (vettore[indice] < vettore_per_vincoli[indice].numero_minimo)
                return 0;
            if ((vettore[indice] > vettore_per_vincoli[indice].numero_minimo) && (vettore_per_vincoli[indice].flag_massimo == 'y'))
                return 0;
        }
    }

    for(i=0; i<NUMERO_CARATTERI; i++)    {
        if(vettore_per_vincoli[i].numero_minimo > 0) {   //vincolo di numero minimo per i caratteri che non sono nella parola, bisogna guardare tutte e 64 le lettere
            if(vettore[i]<vettore_per_vincoli[i].numero_minimo)
                return 0;
        }
    }
    return 1;

}
        1
/** Usato sempre per il comando +inserisci_inizio e convalida subito le parole inserite a seconda dei vincoli appresi in precedenza **/
void inserisci_inizio_filtrato() {
    char input[lunghezza_input];
    struct Nodo *nodo;

    while(1)
    {
        var = scanf("%s",input);
        if(input[0] != '+')
        {
            input[lunghezza_parole] = '\0';
            nodo = malloc(sizeof(struct Nodo));
            nodo->key = malloc(lunghezza_parole_1);
            strcpy(nodo->key,input);
            if (matrice_alberi[fromAsciiToIndex(input[0])][fromAsciiToIndex(input[1])].radice == nil)
            {
                if(((parola_dedotta[0]!='.')&&(parola_dedotta[0]!=input[0])) || ((parola_dedotta[1]!='.')&&(parola_dedotta[1]!=input[1])))
                    matrice_alberi[fromAsciiToIndex(input[0])][fromAsciiToIndex(input[1])].valido=ultimo_n;
            }
            if(compatibile(nodo->key))
                nodo->valido = ultimo_y;
            else
                nodo->valido = ultimo_y;
            inserimento_nell_albero(&matrice_alberi[fromAsciiToIndex(input[0])][fromAsciiToIndex(input[1])],nodo);
        }
        else
            break;
    }
    var++;

}

/** Stampa le parole valide all'interno dell'albero di radice nodo **/
void stampa_albero(struct Nodo *nodo) {
    if(nodo != nil){
        stampa_albero(nodo->left);
        if(nodo->valido != ultimo_n)
            printf("%s\n", nodo->key);
        stampa_albero(nodo->right);
    }
}

/** Gestisce il comando +stampa_filtrate, stampando tutte le parole compatibili coi vincoli e in ordine lessicografico **/
void stampa_filtrate()
{
    short i, j, index0, index1;
    if(parola_dedotta[0] != '.')
    {
        if(parola_dedotta[1] != '.') {       // Se conosco il primo e il secondo carattere posso direttamente stampare un solo albero
            index0=fromAsciiToIndex(parola_dedotta[0]);
            index1=fromAsciiToIndex(parola_dedotta[1]);
            if(matrice_alberi[index0][index1].valido != ultimo_n)
                stampa_albero(matrice_alberi[index0][index1].radice);
        }
        else {                              // Se conosco solo il primo carattere posso direttamente stampare un solo albero
            for(j=0; j<NUMERO_CARATTERI; j++)
            {
                index0= fromAsciiToIndex(parola_dedotta[0]);
                if(matrice_alberi[index0][j].valido != ultimo_n)
                    stampa_albero(matrice_alberi[index0][j].radice);
            }
        }
    }
    // Stesso ragionamento ma considerando solo il secondo carattere
    else if(parola_dedotta[1] != '.') {
        for(j=0; j<NUMERO_CARATTERI; j++)
        {
            index0=fromAsciiToIndex(parola_dedotta[1]);
            if(matrice_alberi[j][index0].valido != ultimo_n)
                stampa_albero(matrice_alberi[j][index0].radice);
        }
    }
    else  // Altrimenti esplorazione completa
        for(i=0; i<NUMERO_CARATTERI; i++)
            for(j=0; j<NUMERO_CARATTERI; j++) {
                if(matrice_alberi[i][j].valido != ultimo_n)
                    stampa_albero(matrice_alberi[i][j].radice);
            }
}

/** Questa funzione si occupa di gestire il comando +inserisci_inizio, inserendo nuove parole che non erano incluse nel dizionario **/
void inserisci_inizio()
{
    char comando[lunghezza_input];
    struct Nodo *nodo;

    while(1)
    {
        var = scanf("%s",comando);
        if(comando[0] != '+') {                           // Se è una parola
            nodo = malloc(sizeof(struct Nodo));       // Creazione nuovo nodo
            nodo->key= malloc(lunghezza_parole_1);
            strcpy(nodo->key, comando);
            nodo->valido = ultimo_y;
            inserimento_nell_albero(&matrice_alberi[fromAsciiToIndex(comando[0])][fromAsciiToIndex(comando[1])],nodo); //Inserimento nell'albero
        }
        else                                               // Se è un comando
            break;
    }
    var++;      // Incremento inutile, per evitare warning del compilatore
}

/** Gestisce una partita **/
void nuova_partita() {
    short i;
    int tentativi;
    char character;

    var = scanf("%s",parola_riferimento);  /** Prendo la parola di riferimento **/

    /** Per ogni possibile carattere inizializzo i suoi vincoli**/
    for(i=0; i<NUMERO_CARATTERI; i++){
        vettore_occorrenze_riferimento[i] = 0;
        vettore_per_vincoli[i].presente = 'y';
        vettore_per_vincoli[i].numero_minimo = 0;
        vettore_per_vincoli[i].flag_massimo = 'n';
        vettore_per_vincoli[i].counter = 0;
        vettore_per_vincoli[i].flag_massimo_counter = 'n';
    }

    /** Analizzo la parola di riferiemnto carattere per carattere, popolando le occorrente nel vettore ricorrenze riferimento
     * Nello stesso ciclo inizializzo la parola dedotta usando dei '.' **/
    for(i=0; i<lunghezza_parole; i++){
        parola_dedotta[i] = '.';
        character = parola_riferimento[i];
        if(character == 45)
            vettore_occorrenze_riferimento[0]++;
        else if (character>47 && character<58)
            vettore_occorrenze_riferimento[character-47]++;
        else if (character>64 && character<91)
            vettore_occorrenze_riferimento[character-54]++;
        else if (character==95)
            vettore_occorrenze_riferimento[37]++;
        else if (character>96 && character<123)
            vettore_occorrenze_riferimento[character-59]++;
    }

    var = scanf("%d",&tentativi);  /** Prendo il numero di tentativi **/

    /** Ora la partita può cominciare **/
    while(tentativi)
    {
        var = scanf("%s",parola_tentativo);  /** Prendo la stringa del tentativo **/
        if(parola_tentativo[0] == '+')  {           // Se è un comando

            if(parola_tentativo[1]=='i')
                inserisci_inizio_filtrato();
            else
                stampa_filtrate();
        }
        else {                                    // Se è una parola tentativo
            if(cerca_parola_tentativo())
            {
                if(confronta_stringhe(parola_tentativo,parola_riferimento) == 0) {
                    printf("ok\n");         /** Parola indovinata, partita vinta **/
                    break;
                }
                else {
                    calcola_vincoli();
                    var = 0; // Azzero var perchè filtra albero la utilizza per un altro scopo
                    filtra();
                    printf("%s\n%d\n", parola_simboli, var);  /** Stampo la parola simboli **/
                    tentativi--;
                }
            }
            else
                printf("not_exists\n");
        }
    }

    if(tentativi == 0)
        printf("ko\n");

    *ultimo_n='y';            /** Per rendere di nuovo tutte le parole valide, cambio il contenuto di ultimo_n con 'y', facendolo puntare a ultimo_y e creo un nuovo ultimo_n **/
    ultimo_n=malloc(1);
    *ultimo_n='n';
}

/** Sulla base dei vincoli appresi, esplora tutte le parole valide rimanenti e aggiorna la validità
 * Questa funzione richiama la funzione filtra, la quale modifica il valore di 'var' in modo che contenga il numero di parole
 * compatibili **/
void filtra() {
    short i,j, index0, index1;
    /** Prima provo a controllare se conosco gia le prime 2 lettere della parola riferimento, poichè velocizzerebbe il
     * processo di ricerca **/
    if(parola_dedotta[0] != '.')
    {
        if(parola_dedotta[1] != '.') {  /** Conosco gia quale albero filtrare **/
            index0 = fromAsciiToIndex(parola_dedotta[0]);
            index1 = fromAsciiToIndex(parola_dedotta[1]);
            if(matrice_alberi[index0][index1].valido != ultimo_n)
                filtra_albero(matrice_alberi[index0][index1].radice);
        }
        else {      /** Non conosco la seconda lettera, ma la prima si, devo filtrare 64 alberi **/
            for(j=0; j<NUMERO_CARATTERI; j++)
            {
                index0 = fromAsciiToIndex(parola_dedotta[0]);
                if(matrice_alberi[index0][j].valido != ultimo_n)
                    filtra_albero(matrice_alberi[index0][j].radice);
            }
        }
    }
    else if(parola_dedotta[1]!='.')  {   /** Non conosco la prima lettera, ma la seconda si, devo filtrare 64 alberi **/
        for(j=0; j<NUMERO_CARATTERI; j++) {
            index0 = fromAsciiToIndex(parola_dedotta[1]);
            if(matrice_alberi[j][index0].valido != ultimo_n)
                filtra_albero(matrice_alberi[j][index0].radice);
        }
    }
    else     /** Non conosce le prime 2 lettere, devo fare una ricerca completa **/
        for(i=0; i<NUMERO_CARATTERI; i++)
            for(j=0; j<NUMERO_CARATTERI; j++) {
                if(matrice_alberi[i][j].valido != ultimo_n)
                    filtra_albero(matrice_alberi[i][j].radice);
            }
}

/** Esplora l'albero identificato dal nodo x e filtra i nodi compatibili con i vincoli
 * Usa la variabile 'var' per contare quante parole sono compatibili **/
void filtra_albero(struct Nodo* x) {
    if(x != nil)
    {
        filtra_albero(x->left);
        if(x->valido != ultimo_n)
        {
            if(compatibile(x->key)==0)
                x->valido = ultimo_n;
            else
                var++;                  /** Var riutilizzato, questa volta come contatore di parole compatibili **/
        }
        filtra_albero(x->right);
    }

}

/** Sulla base della parola tentativo corrente, gestisce i vincoli appresi sulla parola riferimento.
 * Inoltre modifica i conseguenza la parola simboli in modo coerente coi vincoli appresi **/
void calcola_vincoli() {
    short i, indice, j;

    for(i=0; i<lunghezza_parole; i++)  {
        indice = fromAsciiToIndex(parola_tentativo[i]);
        if(vettore_occorrenze_riferimento[indice] == 0) {        /** Se i carattere non è presente nel riferimento **/

            parola_simboli[i] = '/';
            vettore_per_vincoli[indice].presente = 'n';

            /** Se il carattere è il primo o il secondo, posso gia invalidare 64 interi alberi **/
            if(i == 0) {
                for(j=0; j<NUMERO_CARATTERI; j++)
                    matrice_alberi[indice][j].valido = ultimo_n;
            }
            else if(i==1) {
                for(j=0; j<NUMERO_CARATTERI; j++)
                    matrice_alberi[j][indice].valido = ultimo_n;
            }
        }
        else if(parola_tentativo[i] == parola_riferimento[i])  {  /** Se il carattere corrisponde al carattere della parola riferimento **/
            parola_simboli[i] = '+';
            vettore_per_risultato[indice].numero_posizione_corretta++;
            parola_dedotta[i] = parola_tentativo[i];
            vettore_per_vincoli[indice].counter++;
        }
        else
            parola_simboli[i] = '.';
    }

    for(i=0; i<lunghezza_parole; i++)
    {
        indice = fromAsciiToIndex(parola_tentativo[i]);
        if(parola_simboli[i] == '.')
        {
            if (vettore_occorrenze_riferimento[indice] != 0)
            {
                if (vettore_per_risultato[indice].numero_posizione_scorretta >= vettore_occorrenze_riferimento[indice] - vettore_per_risultato[indice].numero_posizione_corretta)
                {
                    parola_simboli[i] = '/';
                    vettore_per_vincoli[indice].flag_massimo_counter = 'y';
                    vettore_per_vincoli[indice].posizioni_vietate[i] = ultimo_n;
                    if(i == 0) {
                        for(j=0; j<NUMERO_CARATTERI; j++)
                            matrice_alberi[indice][j].valido = ultimo_n;
                    }
                    else if(i==1) {
                        for(j=0; j<NUMERO_CARATTERI; j++)
                            matrice_alberi[j][indice].valido = ultimo_n;
                    }
                } else {
                    parola_simboli[i] = '|';
                    vettore_per_vincoli[indice].counter++;
                    vettore_per_vincoli[indice].posizioni_vietate[i] = ultimo_n;
                    if(i == 0) {
                        for(j=0; j<NUMERO_CARATTERI; j++)
                            matrice_alberi[indice][j].valido=ultimo_n;
                    }
                    else if(i == 1) {
                        for(j=0; j<NUMERO_CARATTERI; j++)
                            matrice_alberi[j][indice].valido=ultimo_n;
                    }
                }
                vettore_per_risultato[indice].numero_posizione_scorretta++;
            }
        }
    }

    /** Ripristino vincoli di tentativo e aggiornamento massimi/minimi **/
    for(i=0; i<lunghezza_parole; i++)
    {
        indice = fromAsciiToIndex(parola_tentativo[i]);
        vettore_per_risultato[indice].numero_posizione_corretta = 0;    //Pulizia numero_posizione_corretta
        vettore_per_risultato[indice].numero_posizione_scorretta = 0;   //Pulizia numero_posizione_scorretta
        if(vettore_per_vincoli[indice].presente == 'y')
        {
            if (vettore_per_vincoli[indice].flag_massimo_counter == 'y' && vettore_per_vincoli[indice].flag_massimo == 'n') {
                vettore_per_vincoli[indice].flag_massimo = 'y';
                vettore_per_vincoli[indice].numero_minimo = vettore_per_vincoli[indice].counter;
            } else if (vettore_per_vincoli[indice].flag_massimo_counter == 'n' && vettore_per_vincoli[indice].flag_massimo == 'n') {
                if (vettore_per_vincoli[indice].counter > vettore_per_vincoli[indice].numero_minimo) {
                    vettore_per_vincoli[indice].numero_minimo = vettore_per_vincoli[indice].counter;
                }
            }
            vettore_per_vincoli[indice].flag_massimo_counter = 'n';
            vettore_per_vincoli[indice].counter = 0;
        }
    }
}

/** Restituisce 1 se trova la parola del tentativo nel dizionario
 * Restituisce 0 altrimenti **/
int cerca_parola_tentativo() {
    struct Nodo* root = matrice_alberi[fromAsciiToIndex(parola_tentativo[0])][fromAsciiToIndex(parola_tentativo[1])].radice;
    short value;
    while(root !=  nil)
    {
        value = confronta_stringhe(root->key, parola_tentativo);

        if(value == 1)
            root = root->left;
        else if (value == -1)
            root = root->right;
        else
            return 1;
    }
    return 0;
}

/** Inserisce il dizionario iniziale e returna il secondo carattere del comando scelto, terminato l'inserimento **/
char dizionario_iniziale() {
    char input[lunghezza_input];
    struct Nodo *nodo;

    while(1)
    {
        var = scanf("%s", input);   /** Prende in input la parola **/
        if(input[0]!='+')                 /** Se non è un comando entra nell'if **/
        {
            nodo = malloc(sizeof(struct Nodo));    /** Crea il nodo **/
            nodo->key = malloc(lunghezza_parole_1);    /** Crea la stringa per il nodo **/
            strcpy(nodo->key,input);                   /** Inserisce la striga nel nodo **/
            nodo->valido = ultimo_y;                   /** Il nodo viene marcato valido **/
            inserimento_nell_albero(&matrice_alberi[fromAsciiToIndex(input[0])][fromAsciiToIndex(input[1])],nodo);
        }
        else                              /** Se è un comando entra nell'else, cioè il dizionario è terminato **/
            break;
    }

    var++;
    return input[1];
}

/** Inserimento nell'albero, versione identica agli appunti del corso **/
void inserimento_nell_albero(Albero *T, struct Nodo *z)
{
    struct Nodo *y, *x;
    y = nil;
    x = T->radice;
    while(x != nil) {
        y = x;
        if (confronta_stringhe(z->key, x->key) < 0)
            x = x->left;
        else
            x = x->right;
    }
    if(y == nil)
        T->radice = z;
    else if(confronta_stringhe(z->key, y->key) < 0)
        y->left = z;
    else
        y->right = z;
    z->left = nil;
    z->right = nil;
}

/** Questa funzione confronta l'ordine lessicografico di 2 stringhe
 * Restituisce -1 se la prima stringa viene prima della seconda
 * Restituisce +1 se la prima stringa viene dopo della seconda **/
int confronta_stringhe(char *key, char *key1) {
    for(short i=0;i<lunghezza_parole;i++)
    {
        if(key[i] < key1[i])
            return -1;
        if(key[i] > key1[i])
            return 1;
    }
    return 0;
}

/** Questa funzione converte il codice ascii dei caratteri in indici per i vettori da 64 **/
int fromAsciiToIndex(int ascii){
    if(ascii == 45)
        return 0;
    if (ascii > 47  &&  ascii < 58)
        return ascii-47;
    if (ascii > 64  &&  ascii < 91)
        return ascii-54;
    if (ascii == 95)
        return 37;
    if (ascii > 96  &&  ascii < 123)
        return ascii-59;
    return 0;
}
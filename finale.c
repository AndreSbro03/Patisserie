#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

#define COMMANDMAXLEN 4096

#define STRMAXLEN 255 + 1

typedef char Nome[STRMAXLEN];

typedef struct {
  Nome * tokens;
  size_t len;
} Input;

typedef Nome Ingrediente;

typedef struct {  
  int ingId;
  int qnt;
} CompRicetta;
 
typedef struct {
  CompRicetta * comp;
  size_t len;
} Ricetta;

typedef struct Lotto{
  size_t ingId;
  int qnt;
  int scadenza;
  
  struct Lotto * next;

} lotto_t;

typedef lotto_t * Ptr_lotto;

typedef struct {
  Ptr_lotto lt;
  int qnt;
} Sezione;

// Il magazzino lo gestisco come un array dinamico a cui ogni idice corrisponde un Ingrediente.
// La coppia (ingrediente,indice) verrà salvata in una hash table. l'array dinamico conterrà
// delle linked list di lotti.

typedef struct {
  Sezione * sez;
  size_t len;
} Magazzino;

typedef struct {
  Nome * ing;
  size_t len;
} Ingredienti;

typedef struct {
  Nome nome;
  Ricetta rc;
  int qnt;
  int peso;
  size_t t;
} Ordine;

typedef struct ListaOrdini{
  Ordine ord;
  struct ListaOrdini * next;
} listaordini_t;

typedef listaordini_t * Ptr_ordine;

typedef struct{
  Ptr_ordine buff; //Primo elemento
  Ptr_ordine sp;   //Ultimo elemento
} Coda;

typedef struct {
  int cap;
  size_t t;
  Ordine * buff;
  size_t len;
} Corriere;

enum {
  AGG = 1,
  RMV,
  RIF,
  ORD,
  END,
} Istr; 


typedef struct Cella{
  Nome key;
  Ricetta * ricetta;

  struct Cella * p;
  struct Cella * left;
  struct Cella * right;

} cella_t;

typedef cella_t* Ptr_cella; 

typedef struct{
  Ptr_cella root;
} Albero;


Ptr_cella alloca_cella(){
  Ptr_cella out = malloc(sizeof(cella_t));
  return out;
}

Ptr_cella init_cella(Nome k, Ricetta * rct){
  Ptr_cella out = alloca_cella();
  strcpy(out->key, k);
  out->ricetta = rct;
  out->p = NULL;
  out->left = NULL;
  out->right = NULL;
  return out;
}

// Funzione di hash djb2
unsigned long hash_djb2(Nome str) {
    unsigned long hash = 5381;

    for(size_t i = 0; str[i] != 0; i++){
        hash = ((hash << 5) + hash) + str[i]; // hash * 33 + c
    }

    return hash;
}

/*
  bool valore_minore(Nome k1, Nome k2){
  return hash_djb2(k1) <= hash_djb2(k2);
}
*/


//restituisce true se il valore di k1 è minore di quello di k2, se uguali restituisce false
bool valore_minore(Nome k1, Nome k2){
  
  int i = 0;
  while(k1[i] != '\0'){
    if(k2[i] == '\0') return false; //la seconda stringa è più corta della prima
    
    if(k1[i] != k2[i]) return (k2[i] > k1[i]); //appena trovo due caratteri discordi ritorno il risutato della disquazione

    i++;
  }
  return (k2[i] != '\0'); //se la stringa 2 è non è ancora terminata restituisco ture sennò significa che le due stringhe sono identiche e restituisco false
}

//Cerco la cella contenente una certa chiave nell'albero, se non la trovo ritrono NULL
Ptr_cella cerca_cella(Ptr_cella cl, Nome k){
  if(cl == NULL || strcmp(k, cl -> key) == 0) return cl;

  if(valore_minore(k, cl->key)) 
    return cerca_cella(cl->left, k);
  else 
    return cerca_cella(cl->right, k);

}

void aggiungi_cella(Albero * T, Ptr_cella elem){
  
  Ptr_cella y = NULL;
  Ptr_cella x = T->root;

  // Vado a cercare il punto dell'albero dove andare ad aggiungere la cella
  while(x != NULL){
    y = x;
    if(valore_minore(elem->key, x->key)){
      x = x->left;
    }
    else{
      x = x->right;
    }
  }

  elem->p = y; //y è l'ultimo nodo valido prima di trovare NULL
  
  //Se l'albero è vuoto
  if(y == NULL){
    T->root = elem;
  }
  else if(valore_minore(elem->key, y->key)){
    y->left = elem;  
  }
  else{
    y->right = elem;
  }
}

Ptr_cella cella_minima(Ptr_cella x){
  while(x->left != NULL){
    x = x->left;
  }
  return x;
}

Ptr_cella cella_successiva(Ptr_cella x){
  if(x->right != NULL) {
    return cella_minima(x->right);
  }
  Ptr_cella y = x->p;

  while(y != NULL && x == y->right){
    x = y;
    y = x->p;
  }
  return y;

}

Ptr_cella rimuovi_cella(Albero * T, Ptr_cella z){

  Ptr_cella rmv, temp;

  // Se la cella non ha sotto-alberi allora basta rimuoverla
  // altrimenti andiamo a cercare la cella successiva così da sapere che valore 
  // andare a sostituire in z.
  if(z->left == NULL && z->right == NULL){
    rmv = z;
  }
  else{
    rmv = cella_successiva(z);
  } 
  
  // Se la cella da rimuovere ha un ramo sinistro andiamo a salvare
  // il puntatore al ramo sennò salviamo quello destro 
  if(rmv->left != NULL){
    temp = rmv->left;
  }
  else{
    temp = rmv->right;
  }

  // Andiamo a dire al sotto-albero che adesso la sua cella padre è quella di rmv
  if(temp != NULL){
    temp->p = rmv->p;
  }

  // Se la cella da rimuvore non ha un padre allora significa che bisgna andare a sostituire
  // la radice dell'albero con il nostro sottalbero salvato, sennò se la cella da rimuovere fa 
  // parte di un sottoalbero sinistro/destro mettiamo il sotto-albero a sinistra/destra.
  if(rmv->p == NULL){
    T->root = temp;
  }
  else if(rmv == rmv->p->left){
    rmv->p->left = temp;
  }
  else{
    rmv->p->right = temp;
  }

  // Faccio uno swap dei dati perchè sennò potrei avere problemi con una successiva free
  if(rmv != z){
    Ricetta * t;
    strcpy(z->key,rmv->key);
    t = z->ricetta;
    z->ricetta = rmv->ricetta;
    rmv->ricetta = t;
  }

  return rmv;
}

void dealloca_albero(Ptr_cella x, void (*dealloca_dati) (Ricetta *)){

  if(x != NULL){
    dealloca_albero(x->left, dealloca_dati);
    dealloca_albero(x->right, dealloca_dati);
    //printf("Ho liberato <%s>!\n", x->key);
    if(dealloca_dati != NULL) (*dealloca_dati)(x->ricetta);
    free(x);
  }
}

void stampa_albero(Ptr_cella x){
  if(x != NULL){
    stampa_albero(x->left);
    printf("%s\n", x->key);
    stampa_albero(x->right);
  }
}

Ricetta * cerca_ricetta(Nome nome);
int aggiungi_ricetta(Nome nome, CompRicetta * cr, size_t len);
Input analizza_input();
int esegui_input(Input inp);
CompRicetta * input_to_comp_ricetta(Input inp, size_t * len);
void dealloca_ricetta(Ricetta * rc);
int rimuovi_ricetta(Nome nome);
void espandi_magazzino(int ingId);
Ptr_lotto inserisci_per_scadenza(Ptr_lotto lt, Ptr_lotto testaLt);
void aggiungi_lotto(Ptr_lotto lt);
void dealloca_lotti(Ptr_lotto testaLt);
void dealloca_magazzino();
int aggiungi_ingrediente(Nome ing);
void dealloca_input(Input inp);
void init_corriere(Input inp);
void rimuovi_scaduti(Sezione * sez);
bool ci_sono_ingr(Ordine ord);
Ptr_lotto dealloca_testa(Ptr_lotto testa);
void preleva_ingredienti(Sezione * sez, int qnt);
void aggiungi_ordine(Ordine ord, Coda * cd);
void aggiungi_ordine_in_coda(Ptr_ordine elem, Coda * cd);
void prepara_ordine(Ordine * ord);
void dealloca_ordini(Coda * cd);
bool ricetta_in_coda(Nome nome, Coda cd);
void espandi_corriere();
void rimuovi_ordine_testa(Coda * cd);
void sposta_ordini_corriere();
void carica_corriere();
void ripristina_corriere();

int t = 0;
bool end_program = false;

Albero ricettario = {
  .root = NULL
};

Ingredienti ingredienti = {
  .ing = NULL,
  .len = 0
};

Magazzino magazzino = {
  .sez = NULL,
  .len = 0
};

Corriere corriere = {
  .t = 0,
  .cap = 0,
  .buff = NULL,
  .len = 0
};

Coda pronti = {.buff = NULL, .sp = NULL};
Coda attesa = {.buff = NULL, .sp = NULL};


void stupidsort(int *sorted) {
    int *visited = malloc(corriere.len * sizeof(int));
    for (size_t i = 0; i < corriere.len; i++) {
        visited[i] = 0; // Inizializza l'array dei visitati a 0 (falso)
    }

    for (size_t j = 0; j < corriere.len; j++) {
        int max = -1;
        int maxIdx = -1;

        for (size_t i = 0; i < corriere.len; i++) {
            if (!visited[i] && corriere.buff[i].peso > max) {
                max = corriere.buff[i].peso;
                maxIdx = i;
            }
        }
        sorted[j] = maxIdx;
        visited[maxIdx] = 1; // Segna l'elemento come visitato
    }

    free(visited);
}

int main(){

  Input input = analizza_input();
  init_corriere(input);

  while(!end_program){
    //printf("\ntime: %d\n", t);

    if(t % corriere.t == 0){
      //printf("passa il corriere\n");
      carica_corriere();
      ripristina_corriere();
    }

    input = analizza_input();
    int istr = esegui_input(input);

    if(istr == END) end_program = true;
    else{
      //printf("%d\n", istr);
      t++;
    } 
  }

  /*
  for(size_t i = 0; i < ingredienti.len; ++i){
    printf("%s |", ingredienti.ing[i]);
  }
  printf("\n");
  */

  //stampa_albero(ricettario.root);
  
  dealloca_albero(ricettario.root, &dealloca_ricetta);
  dealloca_magazzino();
  dealloca_ordini(&pronti);
  dealloca_ordini(&attesa);

  free(ingredienti.ing);
  
  return 0; 

}

// Ritorna il numero dell'istruzione eseguita, se negativo allora si è verificato un problema nella relativa isturione
int esegui_input(Input inp){

  int out;
  
  if(inp.len == 0){
    //printf("Input vuoto!\n");
    return END;
  }

  Nome istr;
  strcpy(istr, inp.tokens[0]);
  
  if(strcmp(istr, "aggiungi_ricetta") == 0){

    Nome nome;
    strcpy(nome, inp.tokens[1]);

    if(cerca_ricetta(nome) != NULL){
      //printf("Esiste già una ricetta chiamata \"%s\".\n", nome);
      printf("ignorato\n");
      out = -AGG;
    }
    else{
      size_t len;
      CompRicetta * comp = input_to_comp_ricetta(inp, &len);
      aggiungi_ricetta(nome, comp, len);
      printf("aggiunta\n");
      out = AGG;
    }
  }

  else if(strcmp(istr, "rimuovi_ricetta") == 0){
    
    Nome nome;
    strcpy(nome, inp.tokens[1]);
    if(!ricetta_in_coda(nome, pronti) && !ricetta_in_coda(nome, attesa)){
      if(rimuovi_ricetta(nome) == 0){
        printf("rimossa\n");
      }
      else{
        printf("non presente\n");
      }
    }
    else {
      //printf("Impossibile rimuovere la ricetta \"%s\" in coda.\n", nome);
      printf("ordini in sospeso\n");
    }
    out = RMV;
  }

  else if(strcmp(istr, "rifornimento") == 0){
   
    for(size_t i = 1; i < inp.len; i += 3){

      // vai a prendere l'id dell'ingrediente sapendo il Nome
      int ingId = aggiungi_ingrediente(inp.tokens[i]); 

      Ptr_lotto lt = malloc(sizeof(lotto_t));
        lt->ingId = ingId;
        lt->qnt = atoi(inp.tokens[i + 1]);
        lt->scadenza = atoi(inp.tokens[i + 2]);
        lt->next = NULL;

      // vai a vedere la linked list corrispondente e aggiungi l'ingrediente in ordine 
      // di scadenza
      aggiungi_lotto(lt);
      //printf("Lotto di \"%s\"(%ld) inserito correttamente.\n", inp.tokens[i], lt->ingId);

      Ptr_ordine temp = attesa.buff;
      while(temp != NULL){
        if(ci_sono_ingr(temp->ord)){
          //printf("Preparo l'ordine \"%s\".\n", temp->ord.nome);
          prepara_ordine(&(temp->ord));
          rimuovi_ordine_testa(&attesa);
        }
        temp = attesa.buff;
      }
    }

    printf("rifornito\n");
    out = RIF;
  }

  else if(strcmp(istr, "ordine") == 0){
    
    Ricetta * rc = cerca_ricetta(inp.tokens[1]);
    if(rc == NULL){
      //printf("Non esiste nessuna ricetta chiamata \"%s\".\n", inp.tokens[1]);
      printf("rifiutato\n");
      out = -ORD;
    }
    else{
      Ordine ord;
      strcpy(ord.nome, inp.tokens[1]);
      ord.rc = *rc;
      ord.qnt = atoi(inp.tokens[2]);
      ord.t = t;
      
      if(ci_sono_ingr(ord)){
        //printf("Preparo l'ordine \"%s\".\n", ord.nome);
        prepara_ordine(&ord);
      }
      else{
        //printf("Ordine \"%s\" in attesa.\n", ord.nome);
        aggiungi_ordine(ord, &attesa); 
      }
      printf("accettato\n");
      out = ORD;
    }

  }

  else{
    out = END;
  }
  
  dealloca_input(inp);
  return out;
}


Ricetta * cerca_ricetta(Nome nome) {
  Ptr_cella cl = cerca_cella(ricettario.root, nome);
  if(cl == NULL) return NULL;
  return cl->ricetta;
}

// Controlla se la ricetta esiste già, se no la aggiunge al ricettario
//
// PARAMETRI:
//  - "nome" nome della ricetta
//  - "cr" lista di componenti della ricetta
//
// RITORNA:
//  - 0 se aggiunta correttamente
//  - 1 se la ricetta esiste già
int aggiungi_ricetta(Nome nome, CompRicetta * cr, size_t dim){
  
  if(cerca_ricetta(nome) != NULL){
    printf("[WAR] Ricetta già esistente\n");
    return 1;
  }

  Ricetta * rt = malloc(sizeof(Ricetta)); 
  if(rt == NULL) {
    perror("Buy more ram lol!\n");
    exit(EXIT_FAILURE);
  }

  rt->comp = cr;
  rt->len = dim;

  aggiungi_cella(&ricettario, init_cella(nome, rt));

  //printf("Ricetta \"%s\" aggiunta correttamente.\n", nome);

  return 0;  
}

//Legge una riga di input di lunghezza massima COMMANDMAXLEN e ritorna uno struct contenente la quantità
//di tokens e un puntatore all'array che li contiene
Input analizza_input(){

  Nome * tokens = NULL;
  char input[COMMANDMAXLEN];
  int unused = scanf(" %[^\n]", input);
  (void) unused;

  char * tk = strtok(input, " ");
  int cont = 0;

  while(tk != NULL && strcmp(tk, "\n") != 0){
    tokens = realloc(tokens, sizeof(Nome) * (1 + cont));
    if(tokens == NULL){
      perror("Realloc failed!");
      exit(EXIT_FAILURE);
    }

    strcpy(tokens[cont], strcat(tk,"\0"));

    cont++;
    tk = strtok(NULL, " ");

  }

  Input out = {
    .tokens = tokens,
    .len = cont
  };
  
  return out;
}

// Ritorna l'idice a cui a trovato o aggiunto l'ingrediente
int aggiungi_ingrediente(Nome ing){
  
  if(ingredienti.ing != NULL){
    for(size_t i = 0; i < ingredienti.len; ++i){
      if(strcmp(ingredienti.ing[i], ing) == 0) return i; // ingrediente già esistente
    }
  }
  else {
    ingredienti.ing = malloc(sizeof(Nome));
    ingredienti.len = 0;
  }

  ingredienti.ing = realloc(ingredienti.ing, sizeof(Nome) * (ingredienti.len + 1));
  strcpy(ingredienti.ing[ingredienti.len], ing);

  ingredienti.len++;

  return ingredienti.len - 1;
}

//Riceve in Input ignora i primi due parametri dando per scontato che siano l'istruzione ed il nome della ricetta
//e ritorna un array di componenti della ricetta
CompRicetta * input_to_comp_ricetta(Input inp, size_t * dim){
  size_t numParametri = (inp.len - 2); // Numero di parametri rimasti dopo aver letto l'istruzione ed il Nome
  *dim = numParametri / 2;// Numero di coppie Ingrediente Quantità
  CompRicetta * comp = malloc(sizeof(CompRicetta) * (*dim));

  if(comp == NULL){
    perror("Malloc failed in input_to_comp_ricetta!\n");
    exit(EXIT_FAILURE);
  }

  for(size_t i = 0; i < numParametri; i += 2){
    int idx = i * 0.5;
    comp[idx].ingId = aggiungi_ingrediente(inp.tokens[2 + i]);
    comp[idx].qnt = atoi(inp.tokens[2 + (i + 1)]);
  }

  return comp;
}

void dealloca_ricetta(Ricetta * rc){
    free(rc->comp);
    free(rc);
}

// Rimuove la ricetta se la trova
//  - 1 se non trovata
int rimuovi_ricetta(Nome nome){
  Ptr_cella x = cerca_cella(ricettario.root, nome);

  if(x != NULL){
    x = rimuovi_cella(&ricettario, x);
    dealloca_ricetta(x->ricetta);
    free(x);
    return 0;
  }
  return 1;
}

/*
  GESTIONE DEL MAGAZZINO
*/

void espandi_magazzino(int ingId){
  size_t len = ingId + 1;

  magazzino.sez = realloc(magazzino.sez, len * sizeof(Sezione));
  if(magazzino.sez == NULL){
    perror("Realloc failed!");
    exit(EXIT_FAILURE);
  }

  //memset(magazzino.sez + (magazzino.len * (sizeof(Ptr_lotto))), 0, sizeof(Ptr_lotto)*(len - magazzino.len));

  for(size_t i = magazzino.len; i < len; i++){
    magazzino.sez[i].lt = NULL;
    magazzino.sez[i].qnt = 0;
  }
 
  magazzino.len = len;
}

Ptr_lotto inserisci_per_scadenza(Ptr_lotto lt, Ptr_lotto testaLt){

  if(testaLt == NULL) return lt;

  Ptr_lotto temp, prec = NULL;
  for(temp = testaLt; temp != NULL; temp = temp->next){

    printf("%d\n", temp->scadenza);
    if(lt->scadenza < temp->scadenza){
      lt->next = temp;
      if(prec != NULL){
        prec->next = lt;
        return testaLt;
      }
      else{
        return lt;
      }
    }

    prec = temp;
  }

  prec->next = lt;

  return testaLt;
}

void aggiungi_lotto(Ptr_lotto lt){
  
  // Verifico se esiste una sezione del Magazzino per l'ingrediente, se no lo espando
  if(lt->ingId >= magazzino.len){
    espandi_magazzino(lt->ingId);
  }

  // Vado ad aggiungere alla linked list della sezione il lotto 
  magazzino.sez[lt->ingId].lt = inserisci_per_scadenza(lt, magazzino.sez[lt->ingId].lt);  
  magazzino.sez[lt->ingId].qnt += lt->qnt;
 
}

void dealloca_lotti(Ptr_lotto testaLt){

  Ptr_lotto prec = NULL;
  for(Ptr_lotto temp = testaLt; temp != NULL; temp = temp->next){
    
    if(prec != NULL) free(prec);
    prec = temp;

  }

  if(prec != NULL) free(prec);

}

void dealloca_magazzino(){

  // Dealloca ogni singola serie di lotti
  for(size_t i = 0; i < magazzino.len; ++i){
    dealloca_lotti(magazzino.sez[i].lt);
  }
  // Dealloca le sezioni del magazzino
  free(magazzino.sez);
}

void dealloca_input(Input inp){  
  free(inp.tokens);
}

//inizializza il corriere e libera l'input subito
void init_corriere(Input inp){
  corriere.t = (size_t) atoi(inp.tokens[0]);
  corriere.cap = atoi(inp.tokens[1]);
  dealloca_input(inp);
}

// Rimuove l'elemento e ritorna il successivo
Ptr_lotto dealloca_testa(Ptr_lotto testa){
  if(testa == NULL) return NULL;

  Ptr_lotto out = testa->next;
  free(testa);
  return out;
}

void rimuovi_scaduti(Sezione * sez){
  
  Ptr_lotto temp;
  for(temp = sez->lt; temp != NULL;){
    if(temp->scadenza < t){
      sez->qnt -= temp->qnt;
      temp = dealloca_testa(temp);      
    } 
    else break;
  }
  sez->lt = temp;
}

bool ci_sono_ingr(Ordine ord){
  
  for(size_t i = 0; i < ord.rc.len; ++i){
    size_t id = ord.rc.comp[i].ingId;

    //Rimuovo eventuali elementi scaduti ed aggiorno il contatore degli ingredienti
    rimuovi_scaduti(&magazzino.sez[id]);

    // Se gli ingredienti non sono sufficenti ritorno subito false
    if(magazzino.sez[id].qnt < (ord.rc.comp[i].qnt * ord.qnt)) return false;
  }

  return true;

}

void preleva_ingredienti(Sezione * sez, int qnt){

  int left = qnt;

  for(Ptr_lotto temp = sez->lt; temp != NULL;){
    if(temp->qnt <= left){
      left -= temp->qnt;
      temp = dealloca_testa(temp);
    }
    else{
      temp->qnt -= left;
      left = 0;
    }

    if(left == 0){
      sez->qnt -= qnt;
      return;
    }
  }

  perror("Qualcosa è andato storto in preleva ingredienti.\n");
  exit(EXIT_FAILURE);

  return;
}

void aggiungi_ordine_in_coda(Ptr_ordine elem, Coda * cd){
  if(cd->buff == NULL){
    cd->buff = elem;
  }
  else{
    cd->sp->next = elem;
  }
  cd->sp = elem;
}

void aggiungi_ordine(Ordine ord, Coda * cd){
  Ptr_ordine p = malloc(sizeof(listaordini_t));
  p->ord = ord;
  p->next = NULL;

  aggiungi_ordine_in_coda(p, cd);
 
}


void prepara_ordine(Ordine * _ord){
  // Il check per vedere se ci sono gli ingredienti lo do per già fatto
  
  Ordine ord = *_ord;

  int peso_tot = 0;
  for(size_t i = 0; i < ord.rc.len; ++i){
    int id = ord.rc.comp[i].ingId;
    int peso = ord.rc.comp[i].qnt * ord.qnt; 
    preleva_ingredienti(&magazzino.sez[id], peso); 
    peso_tot += peso;
  }

  _ord->peso = peso_tot;

  //printf("L'ordine \"%s\" è pronto.\n", ord.nome);
  aggiungi_ordine(ord, &pronti);

}

void dealloca_ordini(Coda * cd){

  Ptr_ordine prec = NULL;
  for(Ptr_ordine temp = cd->buff; temp != NULL; temp = temp->next) {
    if(prec != NULL){
      free(prec);
    }
    prec = temp;
  }
  if(prec != NULL){
    free(prec);
  }

  cd->buff = NULL;
  cd->sp = NULL;
}

bool ricetta_in_coda(Nome nome, Coda cd){
  for(Ptr_ordine temp = cd.buff; temp != NULL; temp = temp->next){
    if(strcmp(temp->ord.nome, nome) == 0) return true;
  }
  return false;
}

void espandi_corriere(){

  corriere.buff = realloc(corriere.buff, (corriere.len + 1) * sizeof(Ordine));
  if(corriere.buff  == NULL){
    perror("Realloc failed!");
    exit(EXIT_FAILURE);
  }
  corriere.len++;
}

void rimuovi_ordine_testa(Coda * cd){
  if(cd->buff == NULL) return;
  
  Ptr_ordine trash = cd->buff;
  cd->buff = cd->buff->next;

  if(cd->buff == NULL){
    cd->sp = NULL;
  }

  free(trash);

}

void sposta_ordini_corriere(){

  size_t idx = 0;
  int left = corriere.cap;
  
  Ptr_ordine temp = pronti.buff;
  while( temp != NULL){
    if(temp->ord.peso > left){
      break;
    }
    else{
      espandi_corriere();
      corriere.buff[idx] = temp->ord;
      left -= temp->ord.peso;
      rimuovi_ordine_testa(&pronti);
      ++idx;
    }
    temp = pronti.buff;
  }
}

void carica_corriere(){
  sposta_ordini_corriere();
  int sorted[corriere.len];
  stupidsort(sorted);
  
  if(corriere.len == 0) {
    printf("camioncino vuoto\n");
    return;
  }

  for(size_t i = 0; i < corriere.len; ++i){
    int idx = sorted[i];
    Ordine ord = corriere.buff[idx];
    printf("%ld %s %d\n", ord.t, ord.nome, ord.qnt);
  }
}

void ripristina_corriere(){
  free(corriere.buff);
  corriere.buff = NULL;
  corriere.len = 0;
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

#define STRMAXLEN 255 + 1

typedef struct Nodo{
  int val;
  struct Nodo * next;
} nodo_t;

typedef nodo_t * Ptr_nodo;

typedef char Nome[STRMAXLEN];

typedef struct Token{
  Nome tk; 
  struct Token * next;

} token_t;

typedef token_t * Ptr_token;

typedef struct {
  Ptr_token head;
  Ptr_token tail;
  size_t len;
} Input;

typedef struct {  
  int ingId;
  int qnt;
} CompRicetta;
 
typedef struct {
  Nome nome;
  CompRicetta * comp;
  size_t len;
} Ricetta;

typedef struct {
  Ricetta * rts;
  size_t len;
} Ricettario;

typedef struct Lotto{
  int qnt;
  int scadenza;
  struct Lotto * next;

} lotto_t;

typedef lotto_t * Ptr_lotto;

typedef struct {
  Ptr_lotto lt;
  Nome nomeIng;
  size_t ingId;
  size_t usedBy; // Numero di ricette che hanno come componente questo ingrediente
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
  //Nome nome;
  //TODO: ricontrolla che forse era meglio con la ricetta già salvata
  int rcId; //Id della ricetta
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

// buff = head, sp = tail

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
  int id;

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

Ptr_cella init_cella(Nome k, int id){
  Ptr_cella out = alloca_cella();
  strcpy(out->key, k);
  out->id = id;
  out->p = NULL;
  out->left = NULL;
  out->right = NULL;
  return out;
}

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
  if(z->left == NULL || z->right == NULL){
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
    int t;
    strcpy(z->key,rmv->key);
    t = z->id;
    z->id = rmv->id;
    rmv->id = t;
  }

  return rmv;
}

void dealloca_albero(Ptr_cella x, void (*dealloca_dati) (int)){

  if(x != NULL){
    dealloca_albero(x->left, dealloca_dati);
    dealloca_albero(x->right, dealloca_dati);
    //printf("Ho liberato <%s>!\n", x->key);
    if(dealloca_dati != NULL) (*dealloca_dati)(x->id);
    free(x);
  }
}


// GESTIONE INPUT 
Input analizza_input();
int esegui_input(Input inp);
CompRicetta * input_to_comp_ricetta(Input * inp, size_t * len);
void dealloca_input(Input * inp);
void input_enqueue(Input * inp, Ptr_token data);
Ptr_token input_dequeue(Input * inp);
void input_get_token(Nome out, Input * inp);

// GESTIONE RICETTARIO
int cerca_ricetta(Nome nome);
void aggiungi_ricetta(Nome nome, CompRicetta * cr, size_t len);
void dealloca_ricetta(int id);
int rimuovi_ricetta(Nome nome);
int ricettario_push(Ricetta rt);

// GESTIONE INGREDIENTI 
int aggiungi_ingrediente(Nome ing);
bool ci_sono_ingr(Ordine ord);
Ptr_nodo push_val(Ptr_nodo testa, int val);
Ptr_nodo pop_val(Ptr_nodo testa, int * out);

// GESTIONE MAGAZZINO
void espandi_magazzino(int ingId, Nome nomeIng);
Ptr_lotto inserisci_per_scadenza(Ptr_lotto lt, Ptr_lotto testaLt);
void aggiungi_lotto(Ptr_lotto lt, int ingId);
void dealloca_lotti(Ptr_lotto testaLt);
void dealloca_magazzino();
void rimuovi_scaduti(Sezione * sez, int ingId);
Ptr_lotto dealloca_testa(Ptr_lotto testa);
void preleva_ingredienti(Sezione * sez, int qnt);
void stampa_magazzino();

//GESTIONE CORRIERE
void init_corriere(Input * inp);
void aggiungi_ordine(Ordine ord, Coda * cd, bool rifornimento);
void enqueue(Ptr_ordine elem, Coda * cd);
void prepara_ordine(Ordine ord, bool rifornimento);
void dealloca_ordini(Coda * cd);
bool ricetta_in_coda(int id, Coda cd);
void espandi_corriere();
void dequeue(Coda * cd);
void sposta_ordini_corriere();
void carica_corriere();
void ripristina_corriere();

int t = 0;
bool end_program = false;

Albero idxRicettario = {.root = NULL};
Ricettario ricettario = {.rts = NULL, .len = 0};

Albero ingredienti = {.root = NULL};
int maxId = -1;

Ptr_nodo validIngId = NULL;
Ptr_nodo validRctId = NULL;

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

void insertion_sort(){
  for(size_t j = 1; j < corriere.len; ++j){
    Ordine key = corriere.buff[j];
    int i = (int) j - 1;

    while(i >= 0 && corriere.buff[i].peso < key.peso){
      corriere.buff[i + 1] = corriere.buff[i];
      i--;
    }

    corriere.buff[i + 1] = key;
  }
}

void sort_corriere() {
  insertion_sort();
}

int main(){

  Input input = analizza_input();
  init_corriere(&input);

  while(!end_program){
    //printf("\ntime: %d\n", t);

    if(t % corriere.t == 0 && t != 0){
      //printf("passa il corriere\n");
      carica_corriere();
      ripristina_corriere();
      /* 
      // TODO: Potrebbe rallentare al posto di accellerare 
      for(size_t i = 0; i < magazzino.len; ++i){
        rimuovi_scaduti(&magazzino.sez[i], i);
      }
      */
    }

    input = analizza_input();
    int istr = esegui_input(input);
   
    if(istr == END) end_program = true;
    else{
      t++;
    } 
  }
 
  dealloca_albero(idxRicettario.root, &dealloca_ricetta);
  dealloca_albero(ingredienti.root, NULL);
  dealloca_magazzino();
  dealloca_ordini(&pronti);
  dealloca_ordini(&attesa);

  free(ricettario.rts);
  
  return 0; 

}

void stampa_magazzino(){
  for(size_t i = 0; i < magazzino.len; ++i){
    printf("Sezione %ld avanzano %d grammi.\n", i, magazzino.sez[i].qnt);
  }
}

// Ritorna il numero dell'istruzione eseguita, se negativo allora si è verificato un problema nella relativa isturione
int esegui_input(Input inp){

  int out = 0;
  
  if(inp.len == 0){
    //printf("Input vuoto!\n");
    return END;
  }

  Nome istr;
  input_get_token(istr, &inp);

  if(strcmp(istr, "aggiungi_ricetta") == 0){

    Nome nome;
    input_get_token(nome, &inp);

    if(cerca_ricetta(nome) >= 0){
      //printf("Esiste già una ricetta chiamata \"%s\".\n", nome);
      printf("ignorato\n");
      out = -AGG;
    }
    else{
      size_t len;
      CompRicetta * comp = input_to_comp_ricetta(&inp, &len);
      aggiungi_ricetta(nome, comp, len);
      printf("aggiunta\n");
      out = AGG;
    }
  }

  else if(strcmp(istr, "rimuovi_ricetta") == 0){

    Nome nome;
    input_get_token(nome, &inp);
    int out = rimuovi_ricetta(nome);

    if(out == 0) printf("rimossa\n");
    else if(out == 1) printf("non presente\n");
    else printf("ordini in sospeso\n");

    out = RMV;
  }

  else if(strcmp(istr, "rifornimento") == 0){

    Nome temp;
    for(size_t i = 1; i < inp.len; i += 3){
      // vai a prendere l'id dell'ingrediente sapendo il Nome
      input_get_token(temp, &inp);
      int ingId = aggiungi_ingrediente(temp); 

      Ptr_lotto lt = malloc(sizeof(lotto_t));
      input_get_token(temp, &inp);
      lt->qnt = atoi(temp);
      input_get_token(temp, &inp);
      lt->scadenza = atoi(temp);
      lt->next = NULL;

      // vai a vedere la linked list corrispondente e aggiungi l'ingrediente in ordine 
      // di scadenza
      aggiungi_lotto(lt, ingId);
    }

    // Controlliamo se ci sono ordini sulla lista d'attesa che possono essere preparati
    Ptr_ordine prec = NULL;
    for(Ptr_ordine corr = attesa.buff; corr != NULL; ){
      if(ci_sono_ingr(corr->ord)){
        prepara_ordine(corr->ord, true);

        if(prec != NULL){
          prec->next = corr->next;
          free(corr);
          corr = prec;

          if(prec->next == NULL) attesa.sp = prec; 

          prec = corr;
          corr = corr->next;
        }
        else{
          dequeue(&attesa);
          corr = attesa.buff;
          if(corr == NULL) break;
          prec = NULL;
        }
      }
      else{
        prec = corr;
        corr = corr->next;
      }
    }

    printf("rifornito\n");
    out = RIF;
  }

  else if(strcmp(istr, "ordine") == 0){

    Nome temp;
    input_get_token(temp, &inp);
    int rcId = cerca_ricetta(temp);
    if(rcId < 0){
      //printf("Non esiste nessuna ricetta chiamata \"%s\".\n", inp.tokens[1]);
      printf("rifiutato\n");
      out = -ORD;
    }
    else{
      Ordine ord;
      //strcpy(ord.nome, inp.tokens[1]);
      ord.rcId = rcId;
      input_get_token(temp, &inp);
      ord.qnt = atoi(temp);
      ord.t = t;

      if(ci_sono_ingr(ord)){
        prepara_ordine(ord, false);
      }
      else{
        //printf("Ordine \"%s\" in attesa.\n", ord.nome);
        aggiungi_ordine(ord, &attesa, false); 
      }
      printf("accettato\n");
      out = ORD;
    }

  }

  else{
    out = END;
  }
  
  dealloca_input(&inp);
  return out;
}


int cerca_ricetta(Nome nome) {
  Ptr_cella cl = cerca_cella(idxRicettario.root, nome);
  if(cl == NULL) return -1;
  return cl->id;
}

//TODO: segnare indirizzi ancora validi per evitare crescite superflue
int ricettario_push(Ricetta rt){

  if(validRctId == NULL){
    ricettario.rts = realloc(ricettario.rts, (ricettario.len + 1) * sizeof(Ricetta));
    if(ricettario.rts == NULL){
      perror("Fallita malloc in ricettario_push!\n");
      exit(EXIT_FAILURE);
    }
    memcpy(&ricettario.rts[ricettario.len], &rt, sizeof(Ricetta));

    ricettario.len += 1;
    return ricettario.len - 1;
  }
  else{
    int id = 0;
    validRctId = pop_val(validRctId, &id);
    memcpy(&ricettario.rts[id], &rt, sizeof(Ricetta));
    return id;
  }
}

// Controlla se la ricetta esiste già, se no la aggiunge al ricettario
//
// PARAMETRI:
//  - "nome" nome della ricetta
//  - "cr" lista di componenti della ricetta
//
void aggiungi_ricetta(Nome nome, CompRicetta * cr, size_t dim){

  Ricetta rt;
  rt.comp = cr;
  rt.len = dim;
  strcpy(rt.nome, nome);

  aggiungi_cella(&idxRicettario, init_cella(nome, ricettario_push(rt)));

}

void input_enqueue(Input * inp, Ptr_token data){
  if(inp->head == NULL){
    inp->head = data;
  }
  else{
    inp->tail->next = data;
  }

  inp->tail = data;
}

Ptr_token input_dequeue(Input * inp){
  if(inp->head == NULL || inp->len == 0) return NULL;
  
  Ptr_token out = inp->head;
  inp->head = inp->head->next;
  
  if(inp->head == NULL) {
    inp->tail = NULL;
    //inp->len = 0;
  }

  return out;
}

void input_get_token(Nome out, Input * inp){
  Ptr_token ptk = input_dequeue(inp);
  if(ptk != NULL){
    strcpy(out, ptk->tk);
    free(ptk);
    ptk = NULL;
  }else{
    out[0] = '\0';
  }
}

//Legge una riga di input di lunghezza massima COMMANDMAXLEN e ritorna uno struct contenente la quantità
//di tokens e un puntatore all'array che li contiene
Input analizza_input(){

  Input out = {
    .head = NULL,
    .tail = NULL,
    .len = 0
  };

  bool endCommand = false;
  while(!endCommand){
  
    Nome token = "";
    bool endToken = false;

    for(size_t idx = 0; !endToken; ++idx){

      char x = getchar();

      if(x == '\n' || x == EOF){
        // COMMAND IS FINISH
        endCommand = true;
        x = ' ';
      }      
      if(x == ' '){
        // TOKEN IS END
        endToken = true;
        x = '\0';
      }
      token[idx] = x;
    }

    Ptr_token ptk = malloc(sizeof(token_t));
    if(ptk == NULL){
      perror("Realloc failed!");
      exit(EXIT_FAILURE);
    }

    strcpy(ptk->tk, token);
    ptk->next = NULL;
    input_enqueue(&out, ptk);

    out.len++;
  }

  return out;
}



// Ritorna l'idice a cui ha trovato o aggiunto l'ingrediente
int aggiungi_ingrediente(Nome ing){

  Ptr_cella x = cerca_cella(ingredienti.root, ing);

  if(x == NULL){

    if(validIngId != NULL){
      int id;
      validIngId = pop_val(validIngId, &id);
      aggiungi_cella(&ingredienti, init_cella(ing, id));  
      return id;
    }
    else{
      
      maxId += 1;
      aggiungi_cella(&ingredienti, init_cella(ing, maxId));

      //Espando il magazzino
      if(maxId >= (int) magazzino.len){
        espandi_magazzino(maxId, ing);
      }

      return maxId;
    }
  }
  else return x->id;
}


//Riceve in Input ignora i primi due parametri dando per scontato che siano l'istruzione ed il nome della ricetta
//e ritorna un array di componenti della ricetta
CompRicetta * input_to_comp_ricetta(Input * inp, size_t * dim){
  size_t numParametri = (inp->len - 2); // Numero di parametri rimasti dopo aver letto l'istruzione ed il Nome
  *dim = numParametri / 2;// Numero di coppie Ingrediente Quantità
  CompRicetta * comp = malloc(sizeof(CompRicetta) * (*dim));

  if(comp == NULL){
    perror("Malloc failed in input_to_comp_ricetta!\n");
    exit(EXIT_FAILURE);
  }

  Nome temp;
  for(size_t i = 0; i < numParametri; i += 2){
    int idx = i * 0.5;
    input_get_token(temp, inp);
    comp[idx].ingId = aggiungi_ingrediente(temp);
    magazzino.sez[comp[idx].ingId].usedBy += 1;
    input_get_token(temp, inp);
    comp[idx].qnt = atoi(temp);
  }

  return comp;
}


void dealloca_ricetta(int id){
  free(ricettario.rts[id].comp);
}

// Ritorna la nuova testa
Ptr_nodo push_val(Ptr_nodo testa, int val){
  Ptr_nodo x = malloc(sizeof(nodo_t));
  if(x == NULL){
    perror("Malloc in push_val failed!\n");
    exit(EXIT_FAILURE);
  }
  x->val = val;
  x->next = testa;
  return x;
}

// Ritorna la nuova testa
Ptr_nodo pop_val(Ptr_nodo testa, int * out){
  if(testa == NULL) return NULL;
  Ptr_nodo trash = testa;
  testa = testa->next;
  *out = trash->val;
  free(trash);
  return testa;
}

void decr_used_ingr(int rcId){
  for(size_t i = 0; i < ricettario.rts[rcId].len; ++i){
    size_t ingId = ricettario.rts[rcId].comp[i].ingId;
    Sezione * sez = &magazzino.sez[ingId];
    sez->usedBy -= 1;

    rimuovi_scaduti(sez, ingId);

  }
}

// Rimuove la ricetta se la trova
//  - 0 se rimossa
//  - 1 se non trovata
//  - 2 se ordini in coda
int rimuovi_ricetta(Nome nome){
  Ptr_cella x = cerca_cella(idxRicettario.root, nome);
  
  if(x != NULL){
   if(ricetta_in_coda(x->id, pronti) || ricetta_in_coda(x->id, attesa)){
        return 2;
    }
    else{
      x = rimuovi_cella(&idxRicettario, x);
      decr_used_ingr(x->id);
      dealloca_ricetta(x->id);
      validRctId = push_val(validRctId, x->id);
      free(x);
      return 0;
    }
  }
  return 1;
}

/*
  GESTIONE DEL MAGAZZINO
*/

void espandi_magazzino(int ingId, Nome nomeIng){
  size_t len = ingId + 1;

  magazzino.sez = realloc(magazzino.sez, len * sizeof(Sezione));
  if(magazzino.sez == NULL){
    perror("Realloc failed!");
    exit(EXIT_FAILURE);
  }

  //memset(magazzino.sez + (magazzino.len * (sizeof(Ptr_lotto))), 0, sizeof(Ptr_lotto)*(len - magazzino.len));

  for(size_t i = magazzino.len; i < len; i++){
    strcpy(magazzino.sez[i].nomeIng, nomeIng);
    magazzino.sez[i].usedBy = 0;
    magazzino.sez[i].lt = NULL;
    magazzino.sez[i].qnt = 0;
  }
 
  magazzino.len = len;
}


Ptr_lotto inserisci_per_scadenza(Ptr_lotto lt, Ptr_lotto testaLt){

  if(testaLt == NULL) return lt;

  Ptr_lotto temp, prec = NULL;
  for(temp = testaLt; temp != NULL; temp = temp->next){

   //printf("%d\n", temp->scadenza);
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

void aggiungi_lotto(Ptr_lotto lt, int ingId){
  
  // Vado ad aggiungere alla linked list della sezione il lotto 
  magazzino.sez[ingId].lt = inserisci_per_scadenza(lt, magazzino.sez[ingId].lt);  
  magazzino.sez[ingId].qnt += lt->qnt;
 
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

void dealloca_input(Input * inp){
  while(inp->head != NULL){
    free(input_dequeue(inp));
  }
}

//inizializza il corriere e libera l'input subito
void init_corriere(Input * inp){
  Nome temp;
  input_get_token(temp, inp); 
  corriere.t = (size_t) atoi(temp);
  input_get_token(temp, inp); 
  corriere.cap = atoi(temp);
  dealloca_input(inp);
}

// Rimuove l'elemento e ritorna il successivo
Ptr_lotto dealloca_testa(Ptr_lotto testa){
  if(testa == NULL) return NULL;

  Ptr_lotto out = testa->next;
  free(testa);
  return out;
}

void rimuovi_scaduti(Sezione * sez, int ingId){

  Ptr_lotto temp;
  for(temp = sez->lt; temp != NULL;){
    if(temp->scadenza <= t){
      sez->qnt -= temp->qnt;
      temp = dealloca_testa(temp);      
    } 
    else break;
  }
  sez->lt = temp;

  // Check per vedere se l'ingrediente va rimosso
  if(sez->lt == NULL && sez->usedBy == 0){
    //TODO: aggiungere l'ingId ad una lista di id riciclabili
    Ptr_cella x = cerca_cella(ingredienti.root, sez->nomeIng); 
      if(x != NULL){
        validIngId = push_val(validIngId, ingId);
        free(rimuovi_cella(&ingredienti,x));
        printf("Ho pulito\n");
    }
  }

}

bool ci_sono_ingr(Ordine ord){
 
  Ricetta rc = ricettario.rts[ord.rcId];
  for(size_t i = 0; i < rc.len; ++i){
    size_t id = rc.comp[i].ingId;

    //Rimuovo eventuali elementi scaduti ed aggiorno il contatore degli ingredienti
    if(magazzino.sez[id].lt == NULL) return false;
    rimuovi_scaduti(&magazzino.sez[id], id);

    // Se gli ingredienti non sono sufficenti ritorno subito false
    if(magazzino.sez[id].qnt < (rc.comp[i].qnt * ord.qnt)) return false;
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
      sez->lt = temp;
      sez->qnt -= qnt;
      return;
    }
  }

  perror("Qualcosa è andato storto in preleva ingredienti.\n");
  exit(EXIT_FAILURE);

  return;
}

void enqueue(Ptr_ordine elem, Coda * cd){
  if(cd->buff == NULL){
    cd->buff = elem;
  }
  else{
    cd->sp->next = elem;
  }
  cd->sp = elem;
}

void aggiungi_ordine_tempo(Ptr_ordine elem, Coda * cd){

    bool found = false;
    Ptr_ordine prec = NULL;
    for(Ptr_ordine temp = cd->buff; temp != NULL; temp = temp->next){
      if(elem->ord.t < temp->ord.t){
        if(prec == NULL){
          elem->next = cd->buff;
          cd->buff = elem;
        }
        else{
          prec->next = elem;
          elem->next = temp;
        }
        found = true;
        break;
      }
      prec = temp;
    }
    if(!found){
      enqueue(elem, cd); 
    }
}

void aggiungi_ordine(Ordine ord, Coda * cd, bool rifornimento){
  Ptr_ordine p = malloc(sizeof(listaordini_t));
  p->ord = ord;
  p->next = NULL;
  if(rifornimento){
    aggiungi_ordine_tempo(p, cd);
  }
  else enqueue(p, cd);
 
}


void prepara_ordine(Ordine ord, bool rifornimento){
  // Il check per vedere se ci sono gli ingredienti lo do per già fatto
  Ricetta rc = ricettario.rts[ord.rcId]; 
  int peso_tot = 0;
  for(size_t i = 0; i < rc.len; ++i){
    int id = rc.comp[i].ingId;
    int peso = rc.comp[i].qnt * ord.qnt; 
    preleva_ingredienti(&magazzino.sez[id], peso); 
    peso_tot += peso;
  }

  ord.peso = peso_tot;

  aggiungi_ordine(ord, &pronti, rifornimento);

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

bool ricetta_in_coda(int id, Coda cd){
  for(Ptr_ordine temp = cd.buff; temp != NULL; temp = temp->next){
    if(temp->ord.rcId == id) return true;
  }
  return false;
}

/*
void espandi_corriere(){

  corriere.buff = realloc(corriere.buff, (corriere.len + 1) * sizeof(Ordine));
  if(corriere.buff  == NULL){
    perror("Realloc failed!");
    exit(EXIT_FAILURE);
  }
  corriere.len++;
}
*/

void dequeue(Coda * cd){
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
  int cont = 0;
 
  //TODO: ciclo con complessità (n+1)*(n/2) ~ O(n^2) da rivedere
  for(Ptr_ordine temp = pronti.buff; temp != NULL; temp = temp->next){
    if(temp->ord.peso > left){
      break;
    }
    else {
      left -= temp->ord.peso;
      cont++;
    }
  }

  corriere.len = cont;
  corriere.buff = malloc(sizeof(Ordine) * cont);
  idx = 0;
  left = corriere.cap;

  for(Ptr_ordine temp = pronti.buff; temp != NULL;){
    if(temp->ord.peso > left){
      break;
    }
    else{
      corriere.buff[idx] = temp->ord;
      left -= temp->ord.peso;
      dequeue(&pronti);
      ++idx;
    }
    temp = pronti.buff;
  }
}

void carica_corriere(){
  sposta_ordini_corriere();
  
  if(corriere.len == 0) {
    printf("camioncino vuoto\n");
    return;
  }

  sort_corriere();

  for(size_t i = 0; i < corriere.len; ++i){
    Ordine ord = corriere.buff[i];
    printf("%ld %s %d\n", ord.t, ricettario.rts[ord.rcId].nome, ord.qnt);
  }
}

void ripristina_corriere(){
  free(corriere.buff);
  corriere.buff = NULL;
  corriere.len = 0;
}

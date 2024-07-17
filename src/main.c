#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "myTypes.h"
#include "rbt.c"
#include "algoritmi.h"

// GESTIONE INPUT 
Input get_input_line(Istr istr);
char * get_string(bool * endCommand);
int get_int(bool * endCommand);
void seek_eol();
inpHeader get_input_header();
void esegui_input(inpHeader h);
CompRicetta * input_to_comp_ricetta(Input * inp, uint * len);
void dealloca_input(Input * inp);
void input_enqueue(Input * inp, Ptr_token data);
Ptr_token input_dequeue(Input * inp);
Data input_get_token(Input * inp);
void malloc_failed();

// GESTIONE RICETTARIO
int cerca_ricetta(char * nome);
void aggiungi_ricetta(char * nome, CompRicetta * cr, uint len);
void dealloca_ricetta(int id);
int rimuovi_ricetta(char * nome);
int ricettario_push(Ricetta rt);

// GESTIONE INGREDIENTI 
int aggiungi_ingrediente(char * ing);
bool ci_sono_ingr(Ordine ord, int * missIng);
int controlla_scorte(Ordine ord);
Ptr_nodo push_val(Ptr_nodo testa, int val);
Ptr_nodo pop_val(Ptr_nodo testa, int * out);

// GESTIONE MAGAZZINO
void espandi_magazzino(int ingId, char * nomeIng);
Ptr_lotto inserisci_per_scadenza(Ptr_lotto lt, Ptr_lotto testaLt);
void aggiungi_lotti();
void dealloca_lotti(Ptr_lotto testaLt);
void dealloca_magazzino();
void rimuovi_scaduti(Sezione * sez);
Ptr_lotto dealloca_testa(Ptr_lotto testa);
void preleva_ingredienti(Sezione * sez, int qnt);
void stampa_magazzino();

//GESTIONE CORRIERE
void init_corriere();
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

/*
 * VARIABILI GLOBALI
*/

int t = 0;
bool end_program = false;

Albero idxRicettario = {.root = &Tnil};
Ricettario ricettario = {.rts = NULL, .len = 0};

Albero ingredienti = {.root = &Tnil};
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


void sort_corriere(Corriere * c) {
  quicksort(c->buff, 0, c->len - 1);
  //insertion_sort(c->buff, c->len);
}

/*
 * INIZIO PROGRAMMA
*/

int main(){

  init_corriere();

  while(!end_program){
    //printf("\ntime: %d\n", t);

    if(t % corriere.t == 0 && t != 0){
      carica_corriere();
      ripristina_corriere();
    }

    inpHeader h = get_input_header();
    esegui_input(h);

   
    if(h.istr == END) end_program = true;
    else{
      t++;
    } 
  }
  
  //stampa_albero(idxRicettario.root, 0);
  dealloca_albero(idxRicettario.root, &dealloca_ricetta);
  dealloca_albero(ingredienti.root, NULL);
  dealloca_magazzino();
  dealloca_ordini(&pronti);
  dealloca_ordini(&attesa);

  free(ricettario.rts);
  
  return 0; 

}

// Ritorna il numero dell'istruzione eseguita, se negativo allora si è verificato un problema nella relativa isturione
void esegui_input(inpHeader h){

  Input inp = {.head = NULL, .tail = NULL, .len = 0};
  int out, rcId;
  
  switch (h.istr) {

    case AGG:

      if(cerca_ricetta(h.nome) >= 0){
        //printf("Esiste già una ricetta chiamata \"%s\".\n", nome);
        printf("ignorato\n");
        free(h.nome);
        seek_eol();
      }
      else{
        inp = get_input_line(h.istr);
        uint len;
        CompRicetta * comp = input_to_comp_ricetta(&inp, &len);
        aggiungi_ricetta(h.nome, comp, len);
        printf("aggiunta\n");
      }

      break;

    case RMV:

      out = rimuovi_ricetta(h.nome);

      if(out == 0) printf("rimossa\n");
      else if(out == 1) printf("non presente\n");
      else printf("ordini in sospeso\n");

      free(h.nome);  

      break;

    case RIF:
      
      aggiungi_lotti();
      // Controlliamo se ci sono ordini sulla lista d'attesa che possono essere preparati che mancavano di un ingrediente 
      // che è appena stato rifornito
      Ptr_ordine prec = NULL;
      for(Ptr_ordine corr = attesa.buff; corr != NULL; ){
        
        if(magazzino.sez[corr->ord.missIng].reStock == t){

          int newMissIng = -1;
          if(ci_sono_ingr(corr->ord, &newMissIng)){
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
            corr->ord.missIng = newMissIng;
            prec = corr;
            corr = corr->next;
          }
        }
        else {        
          prec = corr;
          corr = corr->next;
        }
      }

      printf("rifornito\n");
      out = RIF;

      break;

    case ORD:

      rcId = cerca_ricetta(h.nome);
      free(h.nome);

      if(rcId < 0){
        //printf("Non esiste nessuna ricetta chiamata \"%s\".\n", inp.tokens[1]);
        printf("rifiutato\n");
        seek_eol();
      }
      else{

        // Creo l'ordine
        Ordine ord = {
          .rcId = rcId,
          .t = t,
          .missIng = -1,
          .qnt = get_int(NULL)
        };
        
        // Verifico se l'odine può essere preparato. Nel caso procedo immediatamente alla preparazione
        if(ci_sono_ingr(ord, &ord.missIng)) prepara_ordine(ord, false);
        else aggiungi_ordine(ord, &attesa, false); 

        printf("accettato\n");
        out = ORD;
      }

      break;

    case END:
      break;
  }
 
  //dealloca_input(&inp);
}


int cerca_ricetta(char * nome) {
  Ptr_cella cl = cerca_cella(idxRicettario.root, nome);
  if(cl == NULL) return -1;
  return cl->id;
}

int ricettario_push(Ricetta rt){

  if(validRctId == NULL){
    ricettario.rts = realloc(ricettario.rts, (ricettario.len + 1) * sizeof(Ricetta));
    if(ricettario.rts == NULL) malloc_failed();
    memcpy(&ricettario.rts[ricettario.len], &rt, sizeof(Ricetta));
    return ricettario.len++;
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
void aggiungi_ricetta(char * nome, CompRicetta * cr, uint dim){

  Ricetta rt;
  rt.nome = nome;
  rt.comp = cr;
  rt.len = dim;

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
  }

  return out;
}

Data input_get_token(Input * inp){
  Ptr_token ptk = input_dequeue(inp);

  if(ptk != NULL){
    Data out = ptk->tk;
    free(ptk);
    ptk = NULL;
    return out;
  }

  perror("Qualcosa è andato storto in input_get_token\n");
  exit(EXIT_FAILURE);
}

inpHeader get_input_header(){

  inpHeader out = {.istr = END, .nome = NULL};
  
  Nome istr;
  if(scanf("%s", istr) == EOF) return out;

  //La scanf lascia sempre uno spazio dopo oppure un \n, lo skippiamo
  char unused = getchar();
  (void) unused;

  //Sappiamo che il terzo carattere è univoco per ogni comando
  switch (istr[2]) {
    case 'g': 
      //ag[g]iungi
      out.istr = AGG;
      out.nome = get_string(NULL);
      break;
      
    case 'm':
      //ri[m]uovi
      out.istr = RMV;
      out.nome = get_string(NULL);
      break;

    case 'f':
      //ri[f]ornimento
      out.istr = RIF;
      break;

    case 'd':
      //or[d]ine 
      out.istr = ORD;
      out.nome = get_string(NULL);
      break;

    default:
      printf("Comando non riconosciuto");
      break;
  }

  return out;  
}

void seek_eol(){
  while(getchar() != '\n');
}

// Legge una stringa da stdin e copia il contenuto nel puntatore passato come paramentro
// se la stringa era l'ultima della riga o del file ritorna true;
char * get_string(bool * endCommand){
  
  Nome token = "";
  bool endToken = false;
  bool _endCommand = false;
  size_t idx = 0;

  for(;!endToken; ++idx){

    char x = getchar();

    if(x == '\n' || x == EOF){
      // COMMAND IS FINISH
      _endCommand = true;
      x = ' ';
    }      
    if(x == ' '){
      // TOKEN IS END
      endToken = true;
      x = '\0';
    }
    token[idx] = x;
  }

  char * out = memcpy(
    malloc(sizeof(char) * idx), 
    token, 
    sizeof(char) * idx
  );

  if(endCommand != NULL) *endCommand = _endCommand; 

  return out;

}

int read_int(char * cout){
  const int fact = 10;
  int out = 0;
  int pow = 1;
  char c = '\0';
  for(int i = 0; i < STRMAXLEN; ++i){
    c = getchar();
    if(c < '0' || c > '9') break;
    out += (c - '0') * pow;
    pow *= fact;
  }
  *cout = c;
  return out;
}

int get_int(bool * endCommand){
  
  bool _endCommand = false;

  char x;
  int out = read_int(&x);
  _endCommand = (x == '\n' || x == EOF);

  if(endCommand != NULL) *endCommand = _endCommand; 

  return out;

}

void malloc_failed(){
  perror("Malloc failed!");
  exit(EXIT_FAILURE);

}

//type 0 == string, 1 == int
Ptr_token get_token(bool type, bool * endCommand){

  Ptr_token ptk = malloc(sizeof(token_t));
  if(ptk == NULL) malloc_failed();
  
  if(type) ptk->tk.Int = get_int(endCommand);
  else ptk->tk.String = get_string(endCommand);
  ptk->next = NULL;

  return ptk;

}

//Legge una riga di input di lunghezza massima COMMANDMAXLEN e ritorna uno struct contenente la quantità
//di tokens e un puntatore all'array che li contiene
Input get_input_line(Istr istr){

  Input out = {
    .head = NULL,
    .tail = NULL,
    .len = 0
  };

  bool endCommand = false;

  while (!endCommand) {
    //Leggo 1 stringa ed 1 intero
    input_enqueue(&out, get_token(0, NULL));
    input_enqueue(&out, get_token(1, &endCommand));
    out.len += 2;
  }

  return out;
}



// Ritorna l'idice a cui ha trovato o aggiunto l'ingrediente
int aggiungi_ingrediente(char * ing){

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
    
  free(ing);
  return x->id;

}


//Riceve in Input ignora i primi due parametri dando per scontato che siano l'istruzione ed il nome della ricetta
//e ritorna un array di componenti della ricetta
CompRicetta * input_to_comp_ricetta(Input * inp, uint * dim){
  *dim = inp->len / 2;// Numero di coppie Ingrediente Quantità

  CompRicetta * comp = malloc(sizeof(CompRicetta) * (*dim));
  if(comp == NULL) malloc_failed();

  for(size_t i = 0; i < *dim; i += 1){

    char * nome = input_get_token(inp).String;
    comp[i].ingId = aggiungi_ingrediente(nome);
    comp[i].qnt = input_get_token(inp).Int;

  }

  return comp;
}


void dealloca_ricetta(int id){
  free(ricettario.rts[id].comp);
}

// Ritorna la nuova testa
Ptr_nodo push_val(Ptr_nodo testa, int val){
  Ptr_nodo x = malloc(sizeof(nodo_t));
  if(x == NULL) malloc_failed();
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

// Rimuove la ricetta se la trova
//  - 0 se rimossa
//  - 1 se non trovata
//  - 2 se ordini in coda
int rimuovi_ricetta(char * nome){
  Ptr_cella x = cerca_cella(idxRicettario.root, nome);
  
  if(x != NULL){
    if(ricetta_in_coda(x->id, pronti) || ricetta_in_coda(x->id, attesa)){
        return 2;
    }
    else{
      x = rimuovi_cella(&idxRicettario, x);
      dealloca_ricetta(x->id);
      validRctId = push_val(validRctId, x->id);
      free(x->key);
      free(x);
      return 0;
    }
  }
  return 1;
}

/*
  GESTIONE DEL MAGAZZINO
*/

void espandi_magazzino(int ingId, char * nomeIng){
  size_t len = ingId + 1;

  magazzino.sez = realloc(magazzino.sez, len * sizeof(Sezione));
  if(magazzino.sez == NULL) malloc_failed();

  for(size_t i = magazzino.len; i < len; i++){
    magazzino.sez[i].nomeIng = nomeIng;
    magazzino.sez[i].lt = NULL;
    magazzino.sez[i].qnt = 0;
    magazzino.sez[i].reStock = -1;
  }
 
  magazzino.len = len;
}


Ptr_lotto inserisci_per_scadenza(Ptr_lotto lt, Ptr_lotto testaLt){

  if(testaLt == NULL) return lt;

  Ptr_lotto temp, prec = NULL;
  for(temp = testaLt; temp != NULL; temp = temp->next){

    if(lt->scadenza == temp->scadenza)
    {
      temp->qnt += lt->qnt;
      free(lt);
      return testaLt;
    }
    else if(lt->scadenza < temp->scadenza)
    {
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

void aggiungi_lotti(){
  
  bool endCommand = false;

  while (!endCommand) {

    // vai a prendere l'id dell'ingrediente sapendo il Nome
    int ingId = aggiungi_ingrediente(get_string(NULL));

    // Aggiorniamo il valore di restock del magazzino e salviamo l'id in un buffer
    magazzino.sez[ingId].reStock = t;

    Ptr_lotto lt = malloc(sizeof(lotto_t));

    lt->qnt = get_int(NULL);
    lt->scadenza = get_int(&endCommand);
    lt->next = NULL;
    
    // Vado ad aggiungere alla linked list della sezione il lotto 
    magazzino.sez[ingId].qnt += lt->qnt;
    magazzino.sez[ingId].lt = inserisci_per_scadenza(lt, magazzino.sez[ingId].lt);  
  }
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
    Ptr_token x = input_dequeue(inp);
    free(x->tk.String);
    free(x);
  }
}

//inizializza il corriere e libera l'input subito
void init_corriere(){
  scanf("%ud", &corriere.t);
  scanf("%ud", &corriere.cap);
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
    if(temp->scadenza <= t){
      sez->qnt -= temp->qnt;
      temp = dealloca_testa(temp);      
    } 
    else break;
  }
  sez->lt = temp;

}

// Ritorna -1 se ci sono tutte le scorte necessarie per preparare un ordine mentre
// ritorna l'id del primo ingrediente mancante in caso contrario
int controlla_scorte(Ordine ord){

  Ricetta rc = ricettario.rts[ord.rcId];
  for(size_t i = 0; i < rc.len; ++i){
    size_t id = rc.comp[i].ingId;

    //Rimuovo eventuali elementi scaduti ed aggiorno il contatore degli ingredienti
    if(magazzino.sez[id].lt == NULL) return id;
    rimuovi_scaduti(&magazzino.sez[id]);

    // Se gli ingredienti non sono sufficenti ritorno subito false
    if(magazzino.sez[id].qnt < (rc.comp[i].qnt * ord.qnt)) return id;
  }

  return -1;
}


bool ci_sono_ingr(Ordine ord, int * missIng){
  int out = controlla_scorte(ord); 
  if (missIng != NULL) *missIng = out;
  return out == -1;
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

  sort_corriere(&corriere);

  for(size_t i = 0; i < corriere.len; ++i){
    Ordine ord = corriere.buff[i];
    printf("%d %s %d\n", ord.t, ricettario.rts[ord.rcId].nome, ord.qnt);
  }
}

void stampa_magazzino(){
  for(size_t i = 0; i < magazzino.len; ++i){
    printf("Sezione %ld avanzano %d grammi.\n", i, magazzino.sez[i].qnt);
  }
}

void ripristina_corriere(){
  free(corriere.buff);
  corriere.buff = NULL;
  corriere.len = 0;
}

/*
 *
 *
 *  Trovato bug nel passaggio a questo commit:
 *    
 *    $ git checkout 7adfd51
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "myTypes.h"
#include "rbt.h"
#include "algoritmi.h"

#define STATS false

// GESTIONE INPUT 
char * get_token();
int get_int(bool * endCommand);
void seek_eol();
inpHeader get_input_header();
void esegui_input(inpHeader h);
CompRicetta * get_comp_ricetta(uint * len);
void malloc_failed();

// GESTIONE RICETTARIO
int cerca_ricetta(char * nome);
void aggiungi_ricetta(char * nome, CompRicetta * cr, uint len);
void dealloca_ricetta(int id);
int rimuovi_ricetta(char * nome);
int ricettario_push(Ricetta rt);

// GESTIONE INGREDIENTI 
int aggiungi_ingrediente(char * ing);
bool ci_sono_ingr(Ordine ord, int * missIng, int * qntMissIng);
int controlla_scorte(Ordine ord, int * lastIdxComp);
Ptr_nodo push_val(Ptr_nodo testa, int val);
Ptr_nodo pop_val(Ptr_nodo testa, int * out);

// GESTIONE MAGAZZINO
void espandi_magazzino(int ingId);
void aggiungi_lotti();
void dealloca_magazzino();
void rimuovi_scaduti(Sezione * sez);
void preleva_ingredienti(Sezione * sez, int qnt);
void stampa_magazzino();
void stampa_lotti(Arena lts);

//GESTIONE CORRIERE
void init_corriere();
void aggiungi_ordine(Ordine ord, Coda * cd);
void append_arena(Arena * ar, ArenaData data);
void enqueue(Ptr_ordine elem, Coda * cd);
void prepara_ordine(Ordine ord);
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

#define INF (int) 2147483647

int t = 0;
bool end_program = false;

//Albero idxRicettario = {.root = &Tnil};
Ricettario ricettario = {.rts = NULL, .len = 0};

//Albero ingredienti = {.root = &Tnil};
Albero ingredienti = { .root = &Tnil };
Albero idxRicettario = { .root = &Tnil };
int maxId = -1;

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

//Coda pronti = {.buff = NULL, .sp = NULL};
Arena pronti = {.buff = NULL, .len = 0, .size = 0};
Coda attesa = {.buff = NULL, .sp = NULL};

#if STATS
  int num_chiamate_csi = 0;
  int num_successi = 0;
#endif

/*
 * INIZIO PROGRAMMA
*/

int main(){

  init_corriere();

  while(!end_program){
    //printf("\ntime: %d\n", t);

    if(t % corriere.t == 0 && t != 0){
      mergeSort(&pronti.buff->pOrd, 0, pronti.len - 1, TIME_DECR);
      carica_corriere();
      ripristina_corriere();
    }

    inpHeader h = get_input_header();
    esegui_input(h);

    //stampa_lotti(magazzino.sez[0].lts);
    //stampa_magazzino();
    
    if(h.istr == END) end_program = true;
    else{
      t++;
    } 
  }
  
  #if STATS
    printf("Numero chiamate: %d\nNumero successi: %d\n", num_chiamate_csi, num_successi);
    printf("Numero chiamate di csi utili: %f\n", (float) num_successi/num_chiamate_csi);
  #endif

  //stampa_albero(idxRicettario.root, 0);
  dealloca_albero(idxRicettario.root, &dealloca_ricetta);
  dealloca_albero(ingredienti.root, NULL);
  dealloca_magazzino();
  for(uint i = 0; i < pronti.len; i++){
    free((Ptr_ordine) pronti.buff[i].pOrd);
  }
  free(pronti.buff);
  dealloca_ordini(&attesa);

  free(ricettario.rts);
  
  return 0; 

}

// Ritorna il numero dell'istruzione eseguita, se negativo allora si è verificato un problema nella relativa isturione
void esegui_input(inpHeader h){

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
        uint len;
        CompRicetta * comp = get_comp_ricetta(&len);
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

      // Controlliamo se ci sono ordini sulla lista d'attesa che possono essere preparati che mancavano di un ingrediente che è appena stato rifornito
      Ptr_ordine prec = NULL;
      for(Ptr_ordine corr = attesa.buff; corr != NULL; ){
        
        Ricetta rc = ricettario.rts[corr->ord.rcId];
       
        if(
          (magazzino.sez[corr->ord.missIng].reStock == t) &&              // c'è stato un rifornimento dell'ingrediente che mi mancava 
          magazzino.sez[corr->ord.missIng].qnt > 0 &&                     // quell'ingrediente non è già finito
          (rc.maxQnt >= corr->ord.qnt || rc.t != t) &&                    // la quantità massima producibile della ricetta sia maggiore di quella che mi serve
          (magazzino.sez[corr->ord.missIng].qnt >= corr->ord.qntMissIng)  // la quantità che era mancata al missing adesso è disponibile
        ){

          int newMissIng = -1;
          int newQntMissIng = -1;
          if(ci_sono_ingr(corr->ord, &newMissIng, &newQntMissIng)){

            prepara_ordine(corr->ord);

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
            corr->ord.qntMissIng = newQntMissIng;
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
          .qntMissIng = -1,
          .qnt = get_int(NULL)
        };

        // Verifico se l'odine può essere preparato. Nel caso procedo immediatamente alla preparazione
        if(ci_sono_ingr(ord, &ord.missIng, &ord.qntMissIng)) prepara_ordine(ord);
        else aggiungi_ordine(ord, &attesa); 

        printf("accettato\n");
        out = ORD;
      }

      break;

    case END:
      break;
  }
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
    int id;
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
void aggiungi_ricetta(char * nome, CompRicetta * cr, uint len){

  Ricetta rt;
  rt.nome = nome;
  rt.comp = cr;
  rt.len = len;
  rt.t = -1;
  rt.maxQnt = INF;

  aggiungi_cella(&idxRicettario, init_cella(nome, ricettario_push(rt)));

}

inpHeader get_input_header(){

  inpHeader out = {.istr = END, .nome = NULL};
  
  Nome istr;
  if(scanf("%s", istr) == EOF) return out;

  //La scanf lascia sempre uno spazio dopo oppure un \n, lo skippiamo
  char unused = getchar_unlocked();
  (void) unused;

  //Sappiamo che il terzo carattere è univoco per ogni comando
  switch (istr[2]) {
    case 'g': 
      //ag[g]iungi
      out.istr = AGG;
      out.nome = get_token();
      break;
      
    case 'm':
      //ri[m]uovi
      out.istr = RMV;
      out.nome = get_token();
      break;

    case 'f':
      //ri[f]ornimento
      out.istr = RIF;
      break;

    case 'd':
      //or[d]ine 
      out.istr = ORD;
      out.nome = get_token();
      break;

    default:
      printf("Comando non riconosciuto");
      break;
  }

  return out;  
}

void seek_eol(){
  while(getchar_unlocked() != '\n');
}


// Al posto dell'atoi che fa comandi non utili in questo caso come isDigit e che controlla 
// che il numero non sia hex o negativo qui andiamo a eseguire direttamente il comando interessato
// dell'atoi
int get_int(bool * endCommand){
  
  bool endToken = false;
  bool _endCommand = false;
  int out = 0;

  while(!endToken) {

    char x = getchar_unlocked();

    if(x == '\n' || x == EOF){
      // COMMAND IS FINISH
      _endCommand = true;
      x = ' ';
    }      
    if(x == ' '){
      // TOKEN IS END
      endToken = true;
    }
    else {
      out = out * 10 + (x - '0');
    }
  }

  if(endCommand != NULL) *endCommand = _endCommand; 
  return out;
}

// Legge una stringa da stdin e copia il contenuto nel puntatore passato come paramentro
// se la stringa era l'ultima della riga o del file ritorna true;
char * get_token(){
  
  Nome token = "";
  bool endToken = false;
  uint idx = 0;

  for(;!endToken; ++idx){

    char x = getchar_unlocked();

    if(x == ' ' || x == '\n' || x == EOF){
      // TOKEN IS END
      endToken = true;
      x = '\0';
    }
    token[idx] = x;
  }

  char * outString = memcpy(
    malloc(sizeof(char) * idx), 
    token, 
    sizeof(char) * idx
  );

  return outString;

}

int whatTree(char x){
  return x - '0';
}

void malloc_failed(){
  perror("Malloc failed!");
  exit(EXIT_FAILURE);
}

// Ritorna l'idice a cui ha trovato o aggiunto l'ingrediente
int aggiungi_ingrediente(char * ing){

  Ptr_cella x = cerca_cella(ingredienti.root, ing);

  if(x == NULL){
      maxId += 1;
      aggiungi_cella(&ingredienti, init_cella(ing, maxId));

      //Espando il magazzino
      if(maxId >= (int) magazzino.len){
        espandi_magazzino(maxId);
      }

      return maxId;
  }
    
  free(ing);
  return x->id;
}

CompRicetta * get_comp_ricetta(uint * len){
  
  bool endCommand = false;
  CompRicetta * comp = malloc(sizeof(CompRicetta));

  uint idx = 0; 
  while(!endCommand){
    
    if(idx > 0){
      comp = realloc(comp, sizeof(CompRicetta) * (idx + 1));
    }

    comp[idx].ingId = aggiungi_ingrediente(get_token()); 
    comp[idx].qnt = get_int(&endCommand);

    idx++;
  }

  if(len != NULL) *len = idx; 

  return comp;
  
}

void deallocaLL(Ptr_nodo testa){
  while (testa != NULL) {
    Ptr_nodo trash = testa;
    testa = testa->next;
    free(trash);
  }
}

void dealloca_ricetta(int id){
  free(ricettario.rts[id].comp);
  //deallocaLL(ricettario.rts[id].comp);
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
  Ptr_nodo p = testa->next;
  if(out != NULL) *out = testa->val;
  free(testa);
  return p;
}

bool ricetta_in_pronti(int id){
  for(uint i = 0; i < pronti.len; ++i){
    if(pronti.buff[i].pOrd->ord.rcId == id) return true;
  }
  return false;
}

// Rimuove la ricetta se la trova
//  - 0 se rimossa
//  - 1 se non trovata
//  - 2 se ordini in coda
int rimuovi_ricetta(char * nome){
  Ptr_cella x = cerca_cella(idxRicettario.root, nome);
  
  if(x != NULL){
    if(ricetta_in_pronti(x->id) || ricetta_in_coda(x->id, attesa)){
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

void espandi_magazzino(int ingId){
  size_t len = ingId + 1;

  magazzino.sez = realloc(magazzino.sez, len * sizeof(Sezione));
  if(magazzino.sez == NULL) malloc_failed();

  Arena def = {.buff = NULL, .len = 0, .size = 0};

  for(size_t i = magazzino.len; i < len; i++){
    magazzino.sez[i].lts = def;
    magazzino.sez[i].qnt = 0;
    magazzino.sez[i].reStock = -1;
  }
 
  magazzino.len = len;
}

bool bsArena(Arena * ar, ArenaData data, int * minIdx){
  int r = 0;
  int q = ar->len - 1;

  while(r <= q){
    int d = r + ((q - r) * 0.5f);
    if (ar->buff[d].lt.scadenza == data.lt.scadenza){
      ar->buff[d].lt.qnt += data.lt.qnt;
      return true;
    }
    else if(ar->buff[d].lt.scadenza < data.lt.scadenza){
      q = d - 1;
    }
    else{
      if(minIdx != NULL) *minIdx = d;
      r = d + 1;
    }
  }
  
  return false;
}


void aggiungi_lotti(){
  
  bool endCommand = false;

  while (!endCommand) {

    // vai a prendere l'id dell'ingrediente sapendo il Nome
    int ingId = aggiungi_ingrediente(get_token());
    int qnt = get_int(NULL);
    int scad = get_int(&endCommand);

    if(scad > t){
      Sezione * sez = &magazzino.sez[ingId];
      if(sez->reStock != t) rimuovi_scaduti(sez);

      lotto_t lt = {.qnt = qnt, .scadenza = scad};
      
      sez->qnt += qnt;

      int d = 0;
      if(!bsArena(&sez->lts, (ArenaData) lt, &d)){
        if((uint) d >= sez->lts.len){
          append_arena(&sez->lts, (ArenaData) lt);
        }
        else{

          int i = (sez->lts.buff[d].lt.scadenza < scad) ? 0 : 1;

          if(sez->lts.len >= sez->lts.size) sez->lts.buff = realloc(sez->lts.buff, sizeof(ArenaData) * (sez->lts.len + 1));
          memmove(&sez->lts.buff[d + 1 + i], &sez->lts.buff[d + i], sizeof(ArenaData) * (sez->lts.len - d - i));
          sez->lts.buff[d + i].lt = lt;

          sez->lts.len += 1;
          sez->lts.size = sez->lts.len;

        }
      }

      sez->reStock = t;
    }
  }
}

void dealloca_magazzino(){

  // Dealloca ogni singola serie di lotti
  for(size_t i = 0; i < magazzino.len; ++i){
    free(magazzino.sez[i].lts.buff);
  }
  // Dealloca le sezioni del magazzino
  free(magazzino.sez);
}

//inizializza il corriere e libera l'input subito
void init_corriere(){
  corriere.t = get_int(NULL);
  corriere.cap = get_int(NULL);
}

void rimuovi_scaduti(Sezione * sez){
  
  for(int i = sez->lts.len - 1; i >= 0; --i){
    if(sez->lts.buff[i].lt.scadenza <= t){
      sez->qnt -= sez->lts.buff[i].lt.qnt;
      sez->lts.len -= 1;
    }
    else break;
  }
}

// Ritorna -1 se ci sono tutte le scorte necessarie per preparare un ordine mentre
// ritorna l'id del primo ingrediente mancante in caso contrario
int controlla_scorte(Ordine ord, int * idxLastComp){
    
  Ricetta * rc = &ricettario.rts[ord.rcId];
  int maxQnt = INF;

  rc->t = t;

  for(size_t i = 0; i < rc->len; ++i){
    size_t id = rc->comp[i].ingId;
    Sezione * sez = &magazzino.sez[id];
    
    // Non ci sono elementi
    if(sez->qnt <= 0){
      rc->maxQnt = 0;
      if(idxLastComp != NULL) *idxLastComp = i;
      return id;
    }
    
    //Rimuovo eventuali elementi scaduti ed aggiorno il contatore degli ingredienti
    if(sez->reStock != t) {
      rimuovi_scaduti(sez);
      sez->reStock = t;
    }
      
    //Calcoliamo il numero massimo di ricette che possiamo preparare
    int maxRc = sez->qnt / rc->comp[i].qnt;
    if(maxRc < maxQnt) maxQnt = maxRc;

    // se il numero massimo di ricette preparabili non è sufficente termino
    if(maxRc < ord.qnt){
      rc->maxQnt = maxRc;
      if(idxLastComp != NULL) *idxLastComp = i;
      return id;
    } 
  }

  rc->maxQnt = maxQnt;
  if(idxLastComp != NULL) *idxLastComp = -1;
  return -1;

}

bool ci_sono_ingr(Ordine ord, int * missIng, int * qntMissIng){
  #if STATS
    num_chiamate_csi++;
  #endif
  
  int lastIdxComp = -1;
  int out = controlla_scorte(ord, &lastIdxComp); 
  if (missIng != NULL) *missIng = out;
  if (qntMissIng != NULL && lastIdxComp >= 0) *qntMissIng = ricettario.rts[ord.rcId].comp[lastIdxComp].qnt * ord.qnt;

  #if STATS
    if(out == -1) num_successi++;
  #endif
  return out == -1;
}

void preleva_ingredienti(Sezione * sez, int qnt){

  int left = qnt;
  
  for(int i = sez->lts.len - 1; i >= 0; --i){
      
    int * ltQnt = &sez->lts.buff[i].lt.qnt;
    if(*ltQnt <= left){
      left -= *ltQnt;
      sez->lts.len -= 1;
    }
    else{
      *ltQnt -= left;
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

void enqueue(Ptr_ordine elem, Coda * cd){
  if(cd->buff == NULL){
    cd->buff = elem;
  }
  else{
    cd->sp->next = elem;
  }
  cd->sp = elem;
}

void append_arena(Arena * ar, ArenaData data){
  ar->len++;

  if(ar->len <= ar->size){
    ar->buff[ar->len - 1] = data;
    return;
  }
  
  uint newSize = sizeof(ArenaData) * ar->len;
  ar->buff = realloc(ar->buff, newSize);

  ar->buff[ar->len - 1] = data;
  ar->size = ar->len;
}


void aggiungi_ordine(Ordine ord, Coda * cd){
  Ptr_ordine p = malloc(sizeof(listaordini_t));
  p->ord = ord;
  p->next = NULL;
  if(cd == NULL) append_arena(&pronti,(ArenaData) p);
  else enqueue(p, cd);
 
}

void prepara_ordine(Ordine ord){
  // Il check per vedere se ci sono gli ingredienti lo do per già fatto
  Ricetta * rc = &ricettario.rts[ord.rcId]; 
  int peso_tot = 0;
  for(size_t i = 0; i < rc->len; ++i){

    int id = rc->comp[i].ingId;
    int peso = rc->comp[i].qnt * ord.qnt;
    preleva_ingredienti(&magazzino.sez[id], peso); 
    peso_tot += peso;
  }
  
  rc->maxQnt -= ord.qnt;
  ord.peso = peso_tot;

  aggiungi_ordine(ord, NULL);

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

  int left = corriere.cap;
  int cont = 0;
 
  for(int i = pronti.len - 1; i >= 0; --i){
    if(pronti.buff[i].pOrd->ord.peso > left){
      break;
    }
    else {
      left -= pronti.buff[i].pOrd->ord.peso;
      cont++;
    }
  }

  corriere.len = cont;
  corriere.buff = &pronti.buff[pronti.len - cont].pOrd; 
  pronti.len -= cont;

}

void carica_corriere(){
  sposta_ordini_corriere();
  
  if(corriere.len == 0) {
    printf("camioncino vuoto\n");
    return;
  }

  mergeSort(corriere.buff, 0, corriere.len - 1, WEIGHTS_DECR);

  for(size_t i = 0; i < corriere.len; ++i){
    Ptr_ordine pOr = corriere.buff[i];
    printf("%d %s %d\n", pOr->ord.t, ricettario.rts[pOr->ord.rcId].nome, pOr->ord.qnt);
    free(pOr);      
  }
}

void stampa_magazzino(){
  for(size_t i = 0; i < magazzino.len; ++i){
    printf("Sezione %ld avanzano %d grammi.\n", i, magazzino.sez[i].qnt);
  }
}

void stampa_lotti(Arena lts){
  printf("Lotti (%d): ", lts.len);
  for(uint i = 0; i < lts.len; ++i){
    printf("(%d, %d) ", lts.buff[i].lt.qnt, lts.buff[i].lt.scadenza);
  }
  printf("\n");
}

void ripristina_corriere(){
  corriere.buff = NULL;
  corriere.len = 0;
}

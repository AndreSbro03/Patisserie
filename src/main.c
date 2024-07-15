#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "myTypes.c"
#include "bst.c"

// GESTIONE INPUT 
Input analizza_input();
int esegui_input(Input inp);
CompRicetta * input_to_comp_ricetta(Input * inp, uint * len);
void dealloca_input(Input * inp);
void input_enqueue(Input * inp, Ptr_token data);
Ptr_token input_dequeue(Input * inp);
char * input_get_token(Input * inp);

// GESTIONE RICETTARIO
int cerca_ricetta(char * nome);
void aggiungi_ricetta(char * nome, CompRicetta * cr, uint len);
void dealloca_ricetta(int id);
int rimuovi_ricetta(char * nome);
int ricettario_push(Ricetta rt);

// GESTIONE INGREDIENTI 
int aggiungi_ingrediente(char * ing);
bool ci_sono_ingr(Ordine ord);
Ptr_nodo push_val(Ptr_nodo testa, int val);
Ptr_nodo pop_val(Ptr_nodo testa, int * out);

// GESTIONE MAGAZZINO
void espandi_magazzino(int ingId, char * nomeIng);
Ptr_lotto inserisci_per_scadenza(Ptr_lotto lt, Ptr_lotto testaLt);
void aggiungi_lotto(Ptr_lotto lt, int ingId);
void dealloca_lotti(Ptr_lotto testaLt);
void dealloca_magazzino();
void rimuovi_scaduti(Sezione * sez);
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

/*
 * VARIABILI GLOBALI
*/

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

/*
 * INIZIO PROGRAMMA
*/

int main(){

  Input input = analizza_input();
  init_corriere(&input);

  while(!end_program){
    //printf("\ntime: %d\n", t);

    if(t % corriere.t == 0 && t != 0){
      carica_corriere();
      ripristina_corriere();
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

// Ritorna il numero dell'istruzione eseguita, se negativo allora si è verificato un problema nella relativa isturione
int esegui_input(Input inp){

  int out = 0;
  
  if(inp.len == 0){
    //printf("Input vuoto!\n");
    return END;
  }

  char * istr = input_get_token(&inp);
  
  //TODO: se vuoi velocizzare leggermente il tempo puoi al posto che usare strcmp andare semplicemente a vedere la 
  //      terza lettera del comando ("g", "m", "f", "d").
  if(strcmp(istr, "aggiungi_ricetta") == 0){

    char * nome = input_get_token(&inp);

    if(cerca_ricetta(nome) >= 0){
      //printf("Esiste già una ricetta chiamata \"%s\".\n", nome);
      printf("ignorato\n");
      out = -AGG;
      free(nome);
    }
    else{
      uint len;
      CompRicetta * comp = input_to_comp_ricetta(&inp, &len);
      aggiungi_ricetta(nome, comp, len);
      printf("aggiunta\n");
      out = AGG;
    }

    //free(nome);
  }

  else if(strcmp(istr, "rimuovi_ricetta") == 0){

    char * nome = input_get_token(&inp);
    int out = rimuovi_ricetta(nome);

    if(out == 0) printf("rimossa\n");
    else if(out == 1) printf("non presente\n");
    else printf("ordini in sospeso\n");

    free(nome);  
    out = RMV;
  }

  else if(strcmp(istr, "rifornimento") == 0){

    char * temp;
    for(size_t i = 1; i < inp.len; i += 3){
      // vai a prendere l'id dell'ingrediente sapendo il Nome
      temp = input_get_token(&inp);
      int ingId = aggiungi_ingrediente(temp);

      Ptr_lotto lt = malloc(sizeof(lotto_t));

      temp = input_get_token(&inp);
      lt->qnt = atoi(temp);
      free(temp);

      temp = input_get_token(&inp);
      lt->scadenza = atoi(temp);
      free(temp);

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

    char * temp = input_get_token(&inp);
    int rcId = cerca_ricetta(temp);
    free(temp);

    if(rcId < 0){
      //printf("Non esiste nessuna ricetta chiamata \"%s\".\n", inp.tokens[1]);
      printf("rifiutato\n");
      out = -ORD;
    }
    else{
      Ordine ord;
      //strcpy(ord.nome, inp.tokens[1]);
      ord.rcId = rcId;

      temp = input_get_token(&inp);
      ord.qnt = atoi(temp);
      free(temp);

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
 
  free(istr);
  dealloca_input(&inp);
  return out;
}


int cerca_ricetta(char * nome) {
  Ptr_cella cl = cerca_cella(idxRicettario.root, nome);
  if(cl == NULL) return -1;
  return cl->id;
}

int ricettario_push(Ricetta rt){

  if(validRctId == NULL){
    ricettario.rts = realloc(ricettario.rts, (ricettario.len + 1) * sizeof(Ricetta));
    if(ricettario.rts == NULL){
      perror("Fallita malloc in ricettario_push!\n");
      exit(EXIT_FAILURE);
    }
    memcpy(&ricettario.rts[ricettario.len], &rt, sizeof(Ricetta));

    //ricettario.len += 1;
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
    //inp->len = 0;
  }

  return out;
}

char * input_get_token(Input * inp){
  Ptr_token ptk = input_dequeue(inp);
  char * out = NULL;

  if(ptk != NULL){
    out = ptk->tk;
    free(ptk);
    ptk = NULL;
  }

  return out;
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


    ptk->tk = memcpy(
      malloc(sizeof(char) * (strlen(token) + 1)), 
      token, 
      sizeof(char) * (strlen(token) + 1)
    );

    ptk->next = NULL;
    input_enqueue(&out, ptk);

    out.len++;
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
  size_t numParametri = (inp->len - 2); // Numero di parametri rimasti dopo aver letto l'istruzione ed il Nome
  *dim = numParametri / 2;// Numero di coppie Ingrediente Quantità
  CompRicetta * comp = malloc(sizeof(CompRicetta) * (*dim));

  if(comp == NULL){
    perror("Malloc failed in input_to_comp_ricetta!\n");
    exit(EXIT_FAILURE);
  }

  for(size_t i = 0; i < numParametri; i += 2){
    int idx = i * 0.5;

    char * nome = input_get_token(inp);
    comp[idx].ingId = aggiungi_ingrediente(nome);

    //magazzino.sez[comp[idx].ingId].usedBy += 1;
    
    char * qnt = input_get_token(inp);
    comp[idx].qnt = atoi(qnt);
    free(qnt);
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
  if(magazzino.sez == NULL){
    perror("Realloc failed!");
    exit(EXIT_FAILURE);
  }

  //memset(magazzino.sez + (magazzino.len * (sizeof(Ptr_lotto))), 0, sizeof(Ptr_lotto)*(len - magazzino.len));

  for(size_t i = magazzino.len; i < len; i++){
    magazzino.sez[i].nomeIng = nomeIng;
    //magazzino.sez[i].usedBy = 0;
    magazzino.sez[i].lt = NULL;
    magazzino.sez[i].qnt = 0;
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

void aggiungi_lotto(Ptr_lotto lt, int ingId){
  
  // Vado ad aggiungere alla linked list della sezione il lotto 
  magazzino.sez[ingId].qnt += lt->qnt;
  magazzino.sez[ingId].lt = inserisci_per_scadenza(lt, magazzino.sez[ingId].lt);  
  
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
    free(x->tk);
    free(x);
  }
}

//inizializza il corriere e libera l'input subito
void init_corriere(Input * inp){
  char * temp;

  temp = input_get_token(inp); 
  corriere.t = atoi(temp);
  free(temp);

  temp = input_get_token(inp); 
  corriere.cap = atoi(temp);
  free(temp);

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
    if(temp->scadenza <= t){
      sez->qnt -= temp->qnt;
      temp = dealloca_testa(temp);      
    } 
    else break;
  }
  sez->lt = temp;

}

bool ci_sono_ingr(Ordine ord){
 
  Ricetta rc = ricettario.rts[ord.rcId];
  for(size_t i = 0; i < rc.len; ++i){
    size_t id = rc.comp[i].ingId;

    //Rimuovo eventuali elementi scaduti ed aggiorno il contatore degli ingredienti
    if(magazzino.sez[id].lt == NULL) return false;
    rimuovi_scaduti(&magazzino.sez[id]);

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

  sort_corriere();

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

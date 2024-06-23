#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

#include "myTypes.c"
#include "bst.c"

#define COMMANDMAXLEN 1024

Ricetta * cerca_ricetta(Nome nome);
int aggiungi_ricetta(Nome nome, CompRicetta * cr, size_t len);
Input analizza_input();
CompRicetta * input_to_comp_ricetta(Input inp, size_t * len);
void dealloca_ricetta(Ricetta * rc);
void rimuovi_ricetta(Nome nome);
void espandi_magazzino(int ingId);
Ptr_lotto inserisci_per_scadenza(Ptr_lotto lt, Ptr_lotto testaLt);
void aggiungi_lotto(Ptr_lotto lt);
void dealloca_lotti(Ptr_lotto testaLt);
void dealloca_magazzino();
int aggiungi_ingrediente(Nome ing);
void dealloca_input(Input inp);

// Ritorna il numero dell'istruzione eseguita, se negativo allora si è verificato un problema nella relativa isturione
int esegui_input(Input inp){

  int out;
  
  if(inp.len == 0){
    printf("Input vuoto!\n");
    return END;
  }

  Nome istr;
  strcpy(istr, inp.tokens[0]);
  
  if(strcmp(istr, "aggiungi_ricetta") == 0){

    Nome nome;
    strcpy(nome, inp.tokens[1]);

    if(cerca_ricetta(nome) != NULL){
      printf("Esiste già una ricetta chiamata \"%s\".\n", nome);
      out = -AGG;
    }
    else{
      size_t len;
      CompRicetta * comp = input_to_comp_ricetta(inp, &len);
      aggiungi_ricetta(nome, comp, len);
      out = AGG;
    }
  }

  else if(strcmp(istr, "rimuovi_ricetta") == 0){
    
    Nome nome;
    strcpy(nome, inp.tokens[1]);
    //TODO: controllare che non ci siano ordini di tale ricetta ancora da consegnare
    rimuovi_ricetta(nome);
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

      printf("%d\n", lt->scadenza);

      // vai a vedere la linked list corrispondente e aggiungi l'ingrediente in ordine 
      // di scadenza
      aggiungi_lotto(lt);
      printf("Lotto di \"%s\"(%ld) inserito correttamente.\n", inp.tokens[i], lt->ingId);
      
    }

    out = RIF;
  }

  else if(strcmp(istr, "ordine") == 0){
    printf("Ordine ricevuto\n");

    out = ORD;
  }

  else{
    out = END;
  }
  
  dealloca_input(inp);
  return out;
}

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


int main(){
 
  while(!end_program){
    printf("\ntime: %d\n", t);
    Input input = analizza_input();
    int istr = esegui_input(input);

    if(istr == END) end_program = true;
    else{
      printf("%d\n", istr);
      t++;
    } 
  }

  for(size_t i = 0; i < ingredienti.len; ++i){
    //printf("%s |", ingredienti.ing[i]);
  }
  printf("\n");

  stampa_albero(ricettario.root);
  //dealloca_albero(ricettario.root, NULL);
  dealloca_albero(ricettario.root, &dealloca_ricetta);
  dealloca_magazzino();

  free(ingredienti.ing);
  
  return 0; 

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
//  - 2 se la malloc fallisce 
int aggiungi_ricetta(Nome nome, CompRicetta * cr, size_t dim){
  
  if(cerca_ricetta(nome) != NULL){
    printf("[WAR] Ricetta già esistente\n");
    return 1;
  }

  Ricetta * rt = malloc(sizeof(Ricetta)); 
  if(rt == NULL) {
    printf("Buy more ram lol!\n");
    return 2;
  }

  rt->comp = cr;
  rt->len = dim;

  aggiungi_cella(&ricettario, init_cella(nome, rt));

  printf("Ricetta \"%s\" aggiunta correttamente.\n", nome);

  return 0;  
}

//Legge una riga di input di lunghezza massima COMMANDMAXLEN e ritorna uno struct contenente la quantità
//di tokens e un puntatore all'array che li contiene
Input analizza_input(){

  Nome * tokens = NULL;
  char input[COMMANDMAXLEN];
  scanf(" %[^\n]", input);

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
void rimuovi_ricetta(Nome nome){
  Ptr_cella x = cerca_cella(ricettario.root, nome);

  if(x != NULL){
    x = rimuovi_cella(&ricettario, x);
    dealloca_ricetta(x->ricetta);
    free(x);
    printf("Ricetta \"%s\" rimossa correttamente.\n", nome);
  }
  else{
    printf("Nessuna ricetta chiamata \"%s\" trovata.\n", nome);
  }
}

/*
  GESTIONE DEL MAGAZZINO
*/

void espandi_magazzino(int ingId){
  size_t len = ingId + 1;

  magazzino.sez = reallocarray(magazzino.sez, len, sizeof(Ptr_lotto));
  if(magazzino.sez == NULL){
    perror("Realloc failed!");
    exit(EXIT_FAILURE);
  }

  //memset(magazzino.sez + (magazzino.len * (sizeof(Ptr_lotto))), 0, sizeof(Ptr_lotto)*(len - magazzino.len));

  for(size_t i = magazzino.len; i < len; i++){
    magazzino.sez[i] = NULL;
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
  magazzino.sez[lt->ingId] = inserisci_per_scadenza(lt, magazzino.sez[lt->ingId]);  
 
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
    dealloca_lotti(magazzino.sez[i]);
  }
  // Dealloca le sezioni del magazzino
  free(magazzino.sez);
}

void dealloca_input(Input inp){  
  free(inp.tokens);
}

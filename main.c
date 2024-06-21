#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

#include "myTypes.c"
#include "bst.c"

#define COMMANDMAXLEN 1024

Ricetta * cerca_ricetta(Nome nome);
int aggiungi_ricetta(Nome nome, CompRicetta * cr, size_t dim);
Input analizza_input();
CompRicetta * input_to_comp_ricetta(Input inp, size_t * dim);
void dealloca_ricetta(Ricetta * rc);
void rimuovi_ricetta(Nome nome);

// Ritorna il numero dell'istruzione eseguita, se negativo allora si è verificato un problema nella relativa isturione
int esegui_input(Input inp){
  
  if(inp.dim == 0){
    printf("Input vuoto!\n");
    return 1;
  }

  Nome istr;
  strcpy(istr, inp.tokens[0]);
  
  if(strcmp(istr, "aggiungi_ricetta") == 0){

    Nome nome;
    strcpy(nome, inp.tokens[1]);

    if(cerca_ricetta(nome) != NULL){
      printf("Esiste già una ricetta chiamata \"%s\".\n", nome);
      return -AGG;
    }
 
    size_t dim;
    CompRicetta * comp = input_to_comp_ricetta(inp, &dim);
    aggiungi_ricetta(nome, comp, dim);

    //TODO: ricordati di deallocare le varie componenti delle ricette

    return AGG;
  }

  else if(strcmp(istr, "rimuovi_ricetta") == 0){
    
    Nome nome;
    strcpy(nome, inp.tokens[1]);

    rimuovi_ricetta(nome);

    return RMV;
  }

  else if(strcmp(istr, "rifornimento") == 0){
    printf("Rifornito\n");
    return RIF;
  }
  else if(strcmp(istr, "ordine") == 0){
    printf("Ordine ricevuto\n");
    return ORD;
  }
  
  return END;

}

int t = 0;
bool end_program = false;

Albero ricettario = {
  .root = NULL
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

  stampa_albero(ricettario.root);
  //dealloca_albero(ricettario.root, NULL);
  dealloca_albero(ricettario.root, &dealloca_ricetta);
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
  rt->qnt = dim;

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

    strcpy(tokens[cont], tk);

    cont++;
    tk = strtok(NULL, " ");

  }

  Input out = {
    .tokens = tokens,
    .dim = cont
  };
  
  return out;
}

//Riceve in Input ignora i primi due parametri dando per scontato che siano l'istruzione ed il nome della ricetta
//e ritorna un array di componenti della ricetta
CompRicetta * input_to_comp_ricetta(Input inp, size_t * dim){
  size_t numParametri = (inp.dim - 2); // Numero di parametri rimasti dopo aver letto l'istruzione ed il Nome
  *dim = numParametri / 2;// Numero di coppie Ingrediente Quantità
  CompRicetta * comp = malloc(sizeof(CompRicetta) * (*dim));

  if(comp == NULL){
    perror("Malloc failed in input_to_comp_ricetta!\n");
    exit(EXIT_FAILURE);
  }

  for(size_t i = 0; i < numParametri; i += 2){
    strcpy(comp[i].ingr.nome, inp.tokens[1 + i]);
    comp[i].qnt = atoi(inp.tokens[1 + (i + 1)]);
  }

  return comp;
}

void dealloca_ricetta(Ricetta * rc){
    free(rc->comp);
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

#include "myTypes.c"
#include "bst.c"

Ricetta * cerca_ricetta(Nome nome);
int aggiungi_ricetta(Nome nome, CompRicetta * cr);

Nome * analizza_input(){

  Nome * out = malloc(sizeof(Nome));
  if(out == NULL){
    perror("Malloc failed");
    exit(EXIT_FAILURE);
  }

  int cont = 0;

  while (true) {
      char in;
      do{
        scanf("%c", &in);
      } while(in != ' ' && in != :)

      // Verifica se il prossimo carattere è un newline   
      cont++;
      out = realloc(out, sizeof(Nome) * (cont + 1));
  }   

  for(int i = 0; i < cont; i++){
    printf("%s\n", out[i]);
  }
  
  return out;

}

int esegui_input(){

  Nome istr;
  scanf("%s", istr);

  if(strcmp(istr, "aggiungi_ricetta") == 0){

    printf("Aggiunta ricetta\n");
    return AGG;
  }
  else if(strcmp(istr, "rimuovi_ricetta") == 0){
    printf("Rimossa ricetta\n");
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
  
  analizza_input();
  return 0;

#if 0
  while(!end_program){
    printf("\ntime: %d\n", t);
    int istr = analizza_input();

    if(istr == END) end_program = 1;
    else{
      

      printf("%d\n", istr);
      t++;
    }
  }


  return 0; 

#endif

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
int aggiungi_ricetta(Nome nome, CompRicetta * cr){
  
  if(cerca_ricetta(nome) == NULL){
    printf("[WAR] Ricetta già esistente\n");
    return 1;
  }

  Ricetta * rt = malloc(sizeof(Ricetta)); 
  if(rt == NULL) {
    printf("Buy more ram lol!\n");
    return 2;
  }

  rt->comp = cr;

  aggiungi_cella(&ricettario, init_cella(nome, rt));

  return 0;  
}



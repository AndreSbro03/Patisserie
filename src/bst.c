#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

#include "bst.h"

Ptr_cella alloca_cella(){
  Ptr_cella out = malloc(sizeof(cella_t));
  return out;
}

//Lunghezza tenendo conto il carattere terminatore
Ptr_cella init_cella(char * k, int id){
  Ptr_cella out = alloca_cella();
  out->key = k;
  out->id = id;
  out->p = NULL;
  out->left = NULL;
  out->right = NULL;
  return out;
}


//restituisce true se il valore di k1 è minore di quello di k2, se uguali restituisce false
bool valore_minore(char * k1, char * k2){
  
  int i = 0;
  while(k1[i] != '\0'){
    if(k2[i] == '\0') return false; //la seconda stringa è più corta della prima
    
    if(k1[i] != k2[i]) return (k2[i] > k1[i]); //appena trovo due caratteri discordi ritorno il risutato della disquazione

    i++;
  }
  return (k2[i] != '\0'); //se la stringa 2 è non è ancora terminata restituisco ture sennò significa che le due stringhe sono identiche e restituisco false
}

//Cerco la cella contenente una certa chiave nell'albero, se non la trovo ritrono NULL
Ptr_cella cerca_cella(Ptr_cella cl, char * k){
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
    char * rmvKey = rmv->key;
    rmv->key = z->key;
    z->key = rmvKey;

    int t = z->id;
    z->id = rmv->id;
    rmv->id = t;
  }

  free(rmv->key);
  return rmv;
}

void dealloca_albero(Ptr_cella x, void (*dealloca_dati) (int)){

  if(x != NULL){
    dealloca_albero(x->left, dealloca_dati);
    dealloca_albero(x->right, dealloca_dati);
    //printf("Ho liberato <%s>!\n", x->key);
    if(dealloca_dati != NULL) (*dealloca_dati)(x->id);
    free(x->key);
    free(x);
  }
}

void stampa_albero(Ptr_cella x, int k){
  if(x != NULL){
    stampa_albero(x->left, k + 1);
    printf("%d : %d -> %s\n", k, x->id, x->key);
    stampa_albero(x->right, k + 1);
  }
}

#if 0

  int main() {
    
    Albero T = {
      .root = NULL
    };
    Nome n1 = "ciao";
    Nome n2 = "aaaa";
    Nome n3 = "come";
    Nome n4 = "13243564232";
    Nome n5 = "bb";

    aggiungi_cella(&T, init_cella(n1, NULL));
    aggiungi_cella(&T, init_cella(n2, NULL));
    aggiungi_cella(&T, init_cella(n3, NULL));
    aggiungi_cella(&T, init_cella(n4, NULL));
    aggiungi_cella(&T, init_cella(n5, NULL));

    Nome cerca = "ciao";

    Ptr_cella x = cerca_cella(T.root, cerca);
    if(x != NULL){
      printf("Rimosso cella\n");
      x = rimuovi_cella(&T, x);
      free(x);
    }
    
    
    if(cerca_cella(T.root, cerca) != NULL) printf("Trovato!\n");
    else printf("Non trovato!\n");    
    strcpy(cerca, "come");
    if(cerca_cella(T.root, cerca) != NULL) printf("Trovato!\n");
    else printf("Non trovato!\n");
    strcpy(cerca, "fafueioj");
    if(cerca_cella(T.root, cerca) != NULL) printf("Trovato!\n");
    else printf("Non trovato!\n");

    
    stampa_albero(T.root);
    dealloca_albero(T.root, NULL);

    return 0;
  }
#endif

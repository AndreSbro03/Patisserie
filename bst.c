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

void dealloca_albero(Ptr_cella x){

  if(x != NULL){
    dealloca_albero(x->left);
    dealloca_albero(x->right);
    printf("Ho liberato <%s>!\n", x->key);
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

    if(cerca_cella(T.root, "ciao") != NULL) printf("Trovato!\n");
    else printf("Non trovato!\n");
    if(cerca_cella(T.root, "bb") != NULL) printf("Trovato!\n");
    else printf("Non trovato!\n");
    if(cerca_cella(T.root, "feofeonfejnn") != NULL) printf("Trovato!\n");
    else printf("Non trovato!\n");




    stampa_albero(T.root);
    dealloca_albero(T.root);

    return 0;
  }
#endif

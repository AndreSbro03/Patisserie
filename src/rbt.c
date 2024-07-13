#define EXEC 1

#ifdef EXEC
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <stdarg.h>
  #include <stdbool.h>

  #include "myTypes.c"
#endif

#define BLACK true
#define RED false

typedef struct Cella{
  char * key;
  int id;

  bool color;

  struct Cella * p;
  struct Cella * left;
  struct Cella * right;

} cella_t;

typedef cella_t* Ptr_cella; 

typedef struct{
  Ptr_cella root;
  Ptr_cella null;
} Albero;

Ptr_cella alloca_cella();
Ptr_cella init_cella(char * k, int id);
bool valore_minore(char * k1, char * k2);
Ptr_cella cerca_cella(Ptr_cella cl, char * k);
void aggiungi_cella(Albero * T, Ptr_cella elem);
Ptr_cella rimuovi_cella(Albero * T, Ptr_cella z);
void dealloca_albero(Ptr_cella x, void (*dealloca_dati) (int));
void stampa_albero(Ptr_cella x, int k);
void left_rotate(Albero * T, Ptr_cella x);
void right_rotate(Albero * T, Ptr_cella x);
void rb_delete_fixup(Albero * T, Ptr_cella x);
Ptr_cella tree_minimum(Ptr_cella x);
Ptr_cella tree_successor(Ptr_cella x);

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

void rb_insert_fixup(Albero * T, Ptr_cella z){
  if(z == T->root){
    T->root->color = BLACK;
  }
  else{
    Ptr_cella x = z->p;
    if(x->color == RED){
      if(x == x->p->left){

        Ptr_cella y = x->p->right;
        if(y->color == RED){
          x->color = BLACK;
          y->color = BLACK;
          x->p->color = RED;
          rb_insert_fixup(T, x->p);
        }
        else if(z == x->right){
          z = x;
          left_rotate(T, z);
          x = z->p;
          x->color = BLACK;
          x->p->color = RED;
          right_rotate(T, x->p);
        }

      }

      else{

        Ptr_cella y = x->p->left;
        if(y->color == RED){
          x->color = BLACK;
          y->color = BLACK;
          x->p->color = RED;
          rb_insert_fixup(T, x->p);
        }
        else if(z == x->left){
          z = x;
          right_rotate(T, z);
          x = z->p;
          x->color = BLACK;
          x->p->color = RED;
          left_rotate(T, x->p);
        }


      }
    }
  }
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
  if(y == NULL) T->root = elem;
  else if(valore_minore(elem->key, y->key)) y->left = elem;  
  else y->right = elem;

  elem->left = NULL;
  elem->right = NULL;
  elem->color = RED;

  rb_insert_fixup(T, elem);

}

Ptr_cella tree_minimum(Ptr_cella x){
  while(x->left != NULL){
    x = x->left;
  }
  return x;
}

Ptr_cella tree_successor(Ptr_cella x){
  if(x->right != NULL){
    return tree_minimum(x->right);
  }
  Ptr_cella y = x->p;

  while(y != NULL && x == y->right){
    x = y;
    y = y->p;
  }
  return y;
}


Ptr_cella cella_minima(Ptr_cella x){
  while(x->left != NULL){
    x = x->left;
  }
  return x;
}

void left_rotate(Albero * T, Ptr_cella x){
  Ptr_cella y = x->right;
  x->right = y->left;

  if(y->left != NULL) y->left->p = x;

  y->p = x->p;

  if(x->p == NULL) T->root = y;
  else if(x == x->p->left)  x->p->left = y;
  else x->p->right = y;

  y->left = x;
  x->p = y;
}

void right_rotate(Albero * T, Ptr_cella x){
  Ptr_cella y = x->right;
  x->right = y->right;

  if(y->right != NULL) y->left->p = x;

  y->p = x->p;

  if(x->p == NULL) T->root = y;
  else if(x == x->p->right) x->p->left = y;
  else x->p->right = y;

  x->right = x;
  x->p = y;
}

void dealloca_albero(Ptr_cella x, void (*dealloca_dati) (int)){

  if(x != NULL){
    dealloca_albero(x->left, dealloca_dati);
    dealloca_albero(x->right, dealloca_dati);
    //printf("Ho liberato <%s>!\n", x->key);
    if(dealloca_dati != NULL) (*dealloca_dati)(x->id);
    //free(x->key);
    free(x);
  }
}

void stampa_albero_(Ptr_cella x){
  if(x != NULL){
    stampa_albero_(x->left);
    if(!x->color) printf("\033[1;31m"); //Set the text to the color red
    else printf("\033[0m");
    printf("%d -> %s\n", x->id, x->key);
    stampa_albero_(x->right);
  }
}

void stampa_albero(Ptr_cella x, int k){
  if(x != NULL){
    
    stampa_albero(x->left, k + 1);

    if(!x->color) printf("\033[1;31m"); //Set the text to the color red
    else printf("\033[0m");
    printf("%d : %d -> %s\n", k, x->id, x->key);

    stampa_albero(x->right, k + 1);
  }
}

Ptr_cella rimuovi_cella(Albero * T, Ptr_cella z){
  Ptr_cella x, y;

  if(z->left == NULL || z->right == T->null){
    y = z;
  }
  else y = tree_successor(z);
 
  if(y->left != NULL) x = y->left;
  else x = y->right;
  
  if(x != NULL) x->p = y->p;  

  if (y->p == NULL) T->root = x;
  else if (y == y->p->left) y->p->left = x;
  else y->p->right = x;

  if(y != z) z->key = y->key;
  if(y->color == BLACK) rb_delete_fixup(T,x);
  return y;
}

void rb_delete_fixup(Albero * T, Ptr_cella x){

  if(x == NULL) return;  
  if(x->color == RED || x->p == NULL) x->color = BLACK; // Caso 0
  else if(x == x->p->left){

    Ptr_cella w = x->p->right;
  
    if(w->color == RED){
      w->color = BLACK;
  
      x->p->color = RED; // Caso 1
      left_rotate(T,x->p); // Caso 1
      w = x->p->right; // Caso 1
    }
    if(w->left->color == BLACK && w->right->color == BLACK){
      w->color = RED; // Caso 2 
      rb_delete_fixup(T,x->p); // Caso 2
    }
    else if(w->right->color == BLACK){
      w->left->color = BLACK; // Caso 3
      w->color = RED; // Caso 3
      right_rotate(T,w); // Caso 3
      w = x->p->right; // Caso 3
      w->color = x->p->color; // Caso 4
      x->p->color = BLACK; // Caso 4
      w->right->color = BLACK; // Caso 4
      left_rotate(T,x->p); // Caso 4
    }
  }
  
  else{
     Ptr_cella w = x->p->left;
  
    if(w->color == RED){
      w->color = BLACK;
  
      x->p->color = RED; // Caso 1
      right_rotate(T,x->p); // Caso 1
      w = x->p->left; // Caso 1
    }
    if(w->right->color == BLACK && w->left->color == BLACK){
      w->color = RED; // Caso 2 
      rb_delete_fixup(T,x->p); // Caso 2
    }
    else if(w->left->color == BLACK){
      w->right->color = BLACK; // Caso 3
      w->color = RED; // Caso 3
      left_rotate(T,w); // Caso 3
      w = x->p->left; // Caso 3
      w->color = x->p->color; // Caso 4
      x->p->color = BLACK; // Caso 4
      w->left->color = BLACK; // Caso 4
      right_rotate(T,x->p); // Caso 4
    }

  }
}


#ifdef EXEC
  int main() {
   
    Albero T = {
      .root = NULL,
    };

    Nome n1 = "ciao";
    Nome n2 = "aaaa";
    Nome n3 = "come";
    Nome n4 = "13243564232";
    Nome n5 = "bb";

    aggiungi_cella(&T, init_cella(n1, 0));
    aggiungi_cella(&T, init_cella(n2, 0));
    aggiungi_cella(&T, init_cella(n3, 0));
    aggiungi_cella(&T, init_cella(n4, 0));
    aggiungi_cella(&T, init_cella(n5, 0));

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

    
    stampa_albero(T.root, 0);
    dealloca_albero(T.root, NULL);

    return 0;
  }
#endif

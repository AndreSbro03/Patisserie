#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

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

cella_t Tnil = {.left = NULL, .right = NULL, .color = BLACK};

//Lunghezza tenendo conto il carattere terminatore
Ptr_cella init_cella(char * k, int id){
  Ptr_cella out = alloca_cella();
  out->key = k;
  out->id = id;
  out->p = &Tnil;
  out->left = &Tnil;
  out->right = &Tnil;
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
  if(cl == &Tnil || strcmp(k, cl -> key) == 0){
    if(cl == &Tnil) return NULL;
    else return cl;
  }

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
  
  Ptr_cella y = &Tnil;
  Ptr_cella x = T->root;

  // Vado a cercare il punto dell'albero dove andare ad aggiungere la cella
  while(x != &Tnil){
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
  if(y == &Tnil) T->root = elem;
  else if(valore_minore(elem->key, y->key)) y->left = elem;  
  else y->right = elem;
  
  elem->color = RED;

  rb_insert_fixup(T, elem);

}

Ptr_cella tree_minimum(Ptr_cella x){
  while(x->left != &Tnil){
    x = x->left;
  }
  return x;
}

Ptr_cella tree_successor(Ptr_cella x){
  if(x->right != &Tnil){
    return tree_minimum(x->right);
  }
  Ptr_cella y = x->p;

  while(y != &Tnil && x == y->right){
    x = y;
    y = y->p;
  }
  return y;
}


void left_rotate(Albero * T, Ptr_cella x){
  Ptr_cella y = x->right;
  if(y == &Tnil) return;

  x->right = y->left;

  if(y->left != &Tnil) y->left->p = x;

  y->p = x->p;

  if(x->p == &Tnil) T->root = y;
  else if(x == x->p->left)  x->p->left = y;
  else x->p->right = y;

  y->left = x;
  x->p = y;
}

void right_rotate(Albero * T, Ptr_cella x){
  Ptr_cella y = x->left;
  if(y == &Tnil) return;

  x->left = y->right;

  if(y->right != &Tnil) y->right->p = x;

  y->p = x->p;

  if(x->p == &Tnil) T->root = y;
  else if(x == x->p->right) x->p->right = y;
  else x->p->left = y;

  y->right = x;
  x->p = y;
}

void dealloca_albero(Ptr_cella x, void (*dealloca_dati) (int)){

  if(x != &Tnil){
    dealloca_albero(x->left, dealloca_dati);
    dealloca_albero(x->right, dealloca_dati);
    //printf("Ho liberato <%s>!\n", x->key);
    if(dealloca_dati != NULL) (*dealloca_dati)(x->id);
    free(x->key);
    free(x);
  }
}

void stampa_albero_(Ptr_cella x){
  if(x != &Tnil){
    stampa_albero_(x->left);
    if(!x->color) printf("\033[1;31m"); //Set the text to the color red
    else printf("\033[0m");
    printf("%d -> %s\n", x->id, x->key);
    stampa_albero_(x->right);
  }
}

void stampa_albero(Ptr_cella x, int k){
  if(x != &Tnil){
    stampa_albero(x->left, k + 1);
    if(!x->color) printf("\033[1;31m"); //Set the text to the color red
    else printf("\033[0m");
    printf("%d : %d -> %s\n", k, x->id, x->key);
    stampa_albero(x->right, k + 1);
  }
}

Ptr_cella rimuovi_cella(Albero * T, Ptr_cella z){

  Ptr_cella rmv, temp;

  // Se la cella non ha sotto-alberi allora basta rimuoverla
  // altrimenti andiamo a cercare la cella successiva così da sapere che valore 
  // andare a sostituire in z.
  if(z->left == &Tnil || z->right == &Tnil){
    rmv = z;
  }
  else{
    rmv = tree_successor(z);
  } 
  
  // Se la cella da rimuovere ha un ramo sinistro andiamo a salvare
  // il puntatore al ramo sennò salviamo quello destro 
  if(rmv->left != &Tnil){
    temp = rmv->left;
  }
  else{
    temp = rmv->right;
  }

  // Andiamo a dire al sotto-albero che adesso la sua cella padre è quella di rmv
  if(temp != &Tnil){
    temp->p = rmv->p;
  }

  // Se la cella da rimuvore non ha un padre allora significa che bisgna andare a sostituire
  // la radice dell'albero con il nostro sottalbero salvato, sennò se la cella da rimuovere fa 
  // parte di un sottoalbero sinistro/destro mettiamo il sotto-albero a sinistra/destra.
  if(rmv->p == &Tnil){
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

  if(rmv->color == BLACK) rb_delete_fixup(T, temp);
  return rmv;
}

void rb_delete_fixup(Albero * T, Ptr_cella x){

  if(x == &Tnil) return;  
  if(x->color == RED || x->p == &Tnil) x->color = BLACK; // Caso 0
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
  
  else {
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

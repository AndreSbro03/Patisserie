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
int valore_minore(char * k1, char * k2);
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


//restituisce 1 se il valore di k1 è minore di quello di k2, se uguali restituisce 0 altrimenti restituisce -1
int valore_minore(char * k1, char * k2){
  
  int i = 0;
  while(k1[i] != '\0'){
    if(k2[i] == '\0') return -1; //la seconda stringa è più corta della prima
    
    if(k1[i] != k2[i]) return ((k2[i] > k1[i]) ? 1 : -1); //appena trovo due caratteri discordi ritorno il risutato della disquazione

    i++;
  }
  return ((k2[i] != '\0') ? 1 : 0); //se la stringa 2 è non è ancora terminata restituisco true sennò significa che le due stringhe sono identiche e restituisco false
}

//Cerco la cella contenente una certa chiave nell'albero, se non la trovo ritrono NULL
Ptr_cella cerca_cella(Ptr_cella cl, char * k){
  if(cl == &Tnil){
    return NULL;
  }
  
  int out = valore_minore(k, cl->key);
  if(out == 0){
    return cl;
  }
  else if (out > 0){
    return cerca_cella(cl->left, k);
  } 
  else{    
    return cerca_cella(cl->right, k);
  }

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
    if(valore_minore(elem->key, x->key) > 0){
      x = x->left;
    }
    else{
      x = x->right;
    }
  }

  elem->p = y; //y è l'ultimo nodo valido prima di trovare NULL

  //Se l'albero è vuoto
  if(y == &Tnil) T->root = elem;
  else if(valore_minore(elem->key, y->key) > 0) y->left = elem;  
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

typedef enum {
  AGG = 1,
  RMV,
  RIF,
  ORD,
  END,
} Istr;

#define STRMAXLEN 255 + 1

typedef unsigned int uint;

typedef char Nome[STRMAXLEN];

typedef struct {
  Istr istr;
  char * nome;
} inpHeader;

typedef struct {  
  int ingId;
  int qnt;
} CompRicetta;

typedef struct {
  int qnt;
  int scadenza;
} lotto_t;

typedef struct {
  int rcId; //Id della ricetta
  int qnt;
  int peso;
  uint t;

  int missIng; //Id dell'ultimo ingrediente che abbiamo visto mancare (-1 se non manca niente)
} Ordine;

typedef struct ListaOrdini{
  Ordine ord;
  struct ListaOrdini * next;
} listaordini_t;

typedef listaordini_t * Ptr_ordine;

typedef struct{
  Ptr_ordine buff; //Primo elemento
  Ptr_ordine sp;   //Ultimo elemento
} Coda;

// buff = head, sp = tail

typedef struct {
  int cap;
  uint t;
  Ptr_ordine * buff;
  uint len;
} Corriere;

typedef struct Nodo{
  int val;
  struct Nodo * next;
} nodo_t;

typedef nodo_t * Ptr_nodo;

typedef union {
  Ptr_ordine pOrd;
  lotto_t lt;
} AreanaData;

typedef struct {
  AreanaData * buff;
  uint len;
  uint size;
} Arena;

typedef struct {
  char * nome;
  CompRicetta * comp;
  uint len;

  int t;        //ultimo t a cui è stato effettuato un controllo per maxQnt
  int maxQnt;   //massima quantità di ricette preparabili
} Ricetta;

typedef struct {
  Ricetta * rts;
  uint len;
} Ricettario;

typedef struct {
  Arena lts;
  uint ingId;
  int qnt;

  int reStock;    // t dell'ultimo restock 
} Sezione;

// Il magazzino lo gestisco come un array dinamico a cui ogni idice corrisponde un Ingrediente.
// La coppia (ingrediente,indice) verrà salvata in una hash table. l'array dinamico conterrà
// delle linked list di lotti.

typedef struct {
  Sezione * sez;
  uint len;
} Magazzino;

#define WEIGHTS_DECR 0
#define TIME_DECR 1

void merge_l(lotto_t arr[], int l, int m, int r) 
{ 
    int i, j, k; 
    int n1 = m - l + 1; 
    int n2 = r - m; 
  
    // Create temp arrays 
    lotto_t L[n1], R[n2]; 
  
    // Copy data to temp arrays 
    // L[] and R[] 
    for (i = 0; i < n1; i++) 
        L[i] = arr[l + i]; 
    for (j = 0; j < n2; j++) 
        R[j] = arr[m + 1 + j]; 
  
    // Merge the temp arrays back 
    // into arr[l..r] 
    // Initial index of first subarray 
    i = 0; 
  
    // Initial index of second subarray 
    j = 0; 
  
    // Initial index of merged subarray 
    k = l;
    
    while (i < n1 && j < n2) { 
        if (L[i].scadenza >= R[j].scadenza) { 
            arr[k] = L[i]; 
            i++; 
        } 
        else { 
            arr[k] = R[j]; 
            j++; 
        } 
        k++; 
    } 
  
    // Copy the remaining elements 
    // of L[], if there are any 
    while (i < n1) { 
        arr[k] = L[i]; 
        i++; 
        k++; 
    } 
  
    // Copy the remaining elements of 
    // R[], if there are any 
    while (j < n2) { 
        arr[k] = R[j]; 
        j++; 
        k++; 
    } 
} 
  
// l is for left index and r is 
// right index of the sub-array 
// of arr to be sorted 
void mergeSort_l(lotto_t arr[], int l, int r) 
{ 
    if (l < r) { 
        // Same as (l+r)/2, but avoids 
        // overflow for large l and r 
        int m = l + (r - l) / 2; 
  
        // Sort first and second halves 
        mergeSort_l(arr, l, m); 
        mergeSort_l(arr, m + 1, r); 
  
        merge_l(arr, l, m, r); 
    } 
} 

void merge(Ptr_ordine arr[], int l, int m, int r, int type) 
{ 
    int i, j, k; 
    int n1 = m - l + 1; 
    int n2 = r - m; 
  
    // Create temp arrays 
    Ptr_ordine L[n1], R[n2]; 
  
    // Copy data to temp arrays 
    // L[] and R[] 
    for (i = 0; i < n1; i++) 
        L[i] = arr[l + i]; 
    for (j = 0; j < n2; j++) 
        R[j] = arr[m + 1 + j]; 
  
    // Merge the temp arrays back 
    // into arr[l..r] 
    // Initial index of first subarray 
    i = 0; 
  
    // Initial index of second subarray 
    j = 0; 
  
    // Initial index of merged subarray 
    k = l;
  if(type == 1){ // Lavorando con tempo
    
    while (i < n1 && j < n2) { 
        if (L[i]->ord.t >= R[j]->ord.t) { 
            arr[k] = L[i]; 
            i++; 
        } 
        else { 
            arr[k] = R[j]; 
            j++; 
        } 
        k++; 
    } 

  }
  else if(type == 0){ // lavorando con peso
    
    while (i < n1 && j < n2) { 
        if (L[i]->ord.peso > R[j]->ord.peso) { 
            arr[k] = L[i]; 
            i++; 
        }
        else if( L[i]->ord.peso == R[j]->ord.peso) {
            if( L[i]->ord.t < R[j]->ord.t){
                arr[k] = L[i]; 
                i++; 
            }
            else{
                arr[k] = R[j]; 
            j++;
            }
        }
        else { 
            arr[k] = R[j]; 
            j++; 
        } 
        k++; 
    } 
  }
  
    // Copy the remaining elements 
    // of L[], if there are any 
    while (i < n1) { 
        arr[k] = L[i]; 
        i++; 
        k++; 
    } 
  
    // Copy the remaining elements of 
    // R[], if there are any 
    while (j < n2) { 
        arr[k] = R[j]; 
        j++; 
        k++; 
    } 
} 
  
// l is for left index and r is 
// right index of the sub-array 
// of arr to be sorted 
void mergeSort(Ptr_ordine arr[], int l, int r, int type) 
{ 
    if (l < r) { 
        // Same as (l+r)/2, but avoids 
        // overflow for large l and r 
        int m = l + (r - l) / 2; 
  
        // Sort first and second halves 
        mergeSort(arr, l, m, type); 
        mergeSort(arr, m + 1, r, type); 
  
        merge(arr, l, m, r, type); 
    } 
}

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
int whatTree(char x);

// GESTIONE INGREDIENTI 
int aggiungi_ingrediente(char * ing);
bool ci_sono_ingr(Ordine ord, int * missIng);
int controlla_scorte(Ordine ord);
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
void append_arena(Arena * ar, AreanaData data);
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

#define VOCABDIM 'z' - '0' + 1

int t = 0;
bool end_program = false;

//Albero idxRicettario = {.root = &Tnil};
Ricettario ricettario = {.rts = NULL, .len = 0};

//Albero ingredienti = {.root = &Tnil};
Albero ingredienti[VOCABDIM] = { [0 ... VOCABDIM-1].root = &Tnil };
Albero idxRicettario[VOCABDIM] = { [0 ... VOCABDIM-1].root = &Tnil };
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
  for(uint i = 0; i < VOCABDIM; ++i) {
    dealloca_albero(idxRicettario[i].root, &dealloca_ricetta);
    dealloca_albero(ingredienti[i].root, NULL);
  }
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
        if((magazzino.sez[corr->ord.missIng].reStock == t) && (rc.maxQnt >= corr->ord.qnt || rc.t != t)){

          int newMissIng = -1;
          if(ci_sono_ingr(corr->ord, &newMissIng)){

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
        if(ci_sono_ingr(ord, &ord.missIng)) prepara_ordine(ord);
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
  Ptr_cella cl = cerca_cella(idxRicettario[whatTree(nome[0])].root, nome);
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
  rt.maxQnt = 65000;

  aggiungi_cella(&idxRicettario[whatTree(nome[0])], init_cella(nome, ricettario_push(rt)));

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

  int wT = whatTree(ing[0]);
  Ptr_cella x = cerca_cella(ingredienti[wT].root, ing);

  if(x == NULL){
      maxId += 1;
      aggiungi_cella(&ingredienti[wT], init_cella(ing, maxId));

      //Espando il magazzino
      if(maxId >= (int) magazzino.len){
        espandi_magazzino(maxId);
      }

      return maxId;
  }
    
  free(ing);
  return x->id;
}

//Riceve in Input ignora i primi due parametri dando per scontato che siano l'istruzione ed il nome della ricetta
//e ritorna un array di componenti della ricetta
CompRicetta * get_comp_ricetta(uint * len){
  
  const uint defDim = 3; // 3 sembra essere il valore ottimale
  bool endCommand = false;
  CompRicetta * comp = malloc(sizeof(CompRicetta) * defDim);

  uint idx = 0; 
  while(!endCommand){
    
    if(idx >= defDim){
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
  int wT = whatTree(nome[0]); 
  Ptr_cella x = cerca_cella(idxRicettario[wT].root, nome);
  
  if(x != NULL){
    if(ricetta_in_pronti(x->id) || ricetta_in_coda(x->id, attesa)){
        return 2;
    }
    else{
      x = rimuovi_cella(&idxRicettario[wT], x);
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

bool bsArena(Arena * ar, AreanaData data, int * minIdx){
  int r = 0;
  int q = ar->len - 1;

  while(r <= q){
    int d = r + ((q - r) * 0.5f);
    if (ar->buff[d].lt.scadenza == data.lt.scadenza){
      ar->buff[d].lt.qnt += data.lt.qnt;
      if(minIdx != NULL) *minIdx = r;
      return true;
    }
    else if(ar->buff[d].lt.scadenza < data.lt.scadenza){
      q = d - 1;
    }
    else{
      if(minIdx != NULL) *minIdx = r;
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

      int r = 0;
      if(!bsArena(&sez->lts, (AreanaData) lt, &r)){
        append_arena(&sez->lts, (AreanaData) lt);
        if(sez->lts.len != 1) mergeSort_l(&sez->lts.buff->lt, r, sez->lts.len - 1);
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
int controlla_scorte(Ordine ord){
    
  Ricetta * rc = &ricettario.rts[ord.rcId];
  int maxQnt = 65000;

  rc->t = t;

  for(size_t i = 0; i < rc->len; ++i){
    size_t id = rc->comp[i].ingId;
    Sezione * sez = &magazzino.sez[id];
    
    //Rimuovo eventuali elementi scaduti ed aggiorno il contatore degli ingredienti
    if(sez->reStock != t && sez->lts.len > 0) {
      rimuovi_scaduti(sez);
      sez->reStock = t;
    }
      
    //Calcoliamo il numero massimo di ricette che possiamo preparare
    int maxRc = sez->qnt / rc->comp[i].qnt;
    if(maxRc < maxQnt) maxQnt = maxRc;

    // se il numero massimo di ricette preparabili non è sufficente termino
    if(maxRc < ord.qnt){
      rc->maxQnt = maxRc;
      return id;
    } 
  }

  rc->maxQnt = maxQnt;
  return -1;

}

bool ci_sono_ingr(Ordine ord, int * missIng){
  #if STATS
    num_chiamate_csi++;
  #endif

  int out = controlla_scorte(ord); 
  if (missIng != NULL) *missIng = out;

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

void append_arena(Arena * ar, AreanaData data){
  ar->len++;

  if(ar->len <= ar->size){
    ar->buff[ar->len - 1] = data;
    return;
  }
  
  uint newSize = sizeof(AreanaData) * ar->len;
  ar->buff = realloc(ar->buff, newSize);

  ar->buff[ar->len - 1] = data;
  ar->size = ar->len;
}


void aggiungi_ordine(Ordine ord, Coda * cd){
  Ptr_ordine p = malloc(sizeof(listaordini_t));
  p->ord = ord;
  p->next = NULL;
  if(cd == NULL) append_arena(&pronti,(AreanaData) p);
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

  //Prendiamo la lista di ordini pronti e la splittiamo in due nuovi array, uno di ordini da spedire e uno di ordini ancora pronti ma che non stavano nel corriere.
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

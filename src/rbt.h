/*
*
* Liberia per la gestione di alberi rosso neri adattata al progetto del corso del Politecnico di Milano: API 2023/2024.
* Le funzioni sono stato riaddattate basandosi sulle slides fornite dal professor Martinenghi.
* 
* Sviluppata da ANDREA SBROGIO'.
*
* Si prega di non rimuovere i riconoscimenti dal file se si ha intenzione di ripibblicare il codice sottostante.
* Per il resto sentiteci liberi di farci ciò che volete.*
*
 */
#include <stdlib.h> 

// Definizione dei colori
#define BLACK true 
#define RED false

typedef struct Cella{
  char * key;             // Stringa univoca (ingrediente o ricetta)
  int id;                 // Posizione dell'oggetto all'interno della HashTable corrispondente

  bool color;             // Colore della cella

  struct Cella * p;       // Cella padre
  struct Cella * left;    // Cella sinistra
  struct Cella * right;   // Cella destra

} cella_t;

typedef cella_t* Ptr_cella; 

typedef struct{
  Ptr_cella root;
} Albero;

extern cella_t Tnil;

Ptr_cella alloca_cella();                                       // Alloca lo spazio necessario per una cella
Ptr_cella init_cella(char * k, int id);                         // Inizializza la cella con la chiave e l'id fornito
int valore_minore(char * k1, char * k2);                        // Funzione per la comparazione di stringhe
Ptr_cella cerca_cella(Ptr_cella cl, char * k);                  // Cerca la cella con la chiave (k) fornita
void aggiungi_cella(Albero * T, Ptr_cella elem);                
Ptr_cella rimuovi_cella(Albero * T, Ptr_cella z);
void dealloca_albero(Ptr_cella x, void (*dealloca_dati) (int));
void stampa_albero(Ptr_cella x, int k);
void left_rotate(Albero * T, Ptr_cella x);
void right_rotate(Albero * T, Ptr_cella x);
void rb_delete_fixup(Albero * T, Ptr_cella x);
Ptr_cella tree_minimum(Ptr_cella x);
Ptr_cella tree_successor(Ptr_cella x);


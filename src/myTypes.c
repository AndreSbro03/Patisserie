#define STRMAXLEN 255 + 1

typedef unsigned int uint;

typedef struct Nodo{
  int val;
  struct Nodo * next;
} nodo_t;

typedef nodo_t * Ptr_nodo;

typedef char Nome[STRMAXLEN];

typedef struct Token{
  char * tk; 
  struct Token * next;

} token_t;

typedef token_t * Ptr_token;

typedef struct {
  Ptr_token head;
  Ptr_token tail;
  uint len;
} Input;

typedef struct {  
  int ingId;
  int qnt;
} CompRicetta;
 
typedef struct {
  char * nome;
  CompRicetta * comp;
  uint len;
} Ricetta;

typedef struct {
  Ricetta * rts;
  uint len;
} Ricettario;

typedef struct Lotto{
  int qnt;
  int scadenza;
  struct Lotto * next;

} lotto_t;

typedef lotto_t * Ptr_lotto;

typedef struct {
  Ptr_lotto lt;
  char * nomeIng;
  uint ingId;
  //uint usedBy; // Numero di ricette che hanno come componente questo ingrediente
  int qnt;
} Sezione;

// Il magazzino lo gestisco come un array dinamico a cui ogni idice corrisponde un Ingrediente.
// La coppia (ingrediente,indice) verrà salvata in una hash table. l'array dinamico conterrà
// delle linked list di lotti.

typedef struct {
  Sezione * sez;
  uint len;
} Magazzino;

typedef struct {
  //TODO: ricontrolla che forse era meglio con la ricetta già salvata
  int rcId; //Id della ricetta
  int qnt;
  int peso;
  uint t;
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
  Ordine * buff;
  uint len;
} Corriere;

enum {
  AGG = 1,
  RMV,
  RIF,
  ORD,
  END,
} Istr; 

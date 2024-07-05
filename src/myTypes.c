#define STRMAXLEN 255 + 1

typedef char Nome[STRMAXLEN];

typedef struct {
  Nome * tokens;
  size_t len;
} Input;

typedef Nome Ingrediente;

typedef struct {  
  int ingId;
  int qnt;
} CompRicetta;
 
typedef struct {
  CompRicetta * comp;
  size_t len;
} Ricetta;

typedef struct Lotto{
  size_t ingId;
  int qnt;
  int scadenza;
  
  struct Lotto * next;

} lotto_t;

typedef lotto_t * Ptr_lotto;

typedef struct {
  Ptr_lotto lt;
  int qnt;
} Sezione;

// Il magazzino lo gestisco come un array dinamico a cui ogni idice corrisponde un Ingrediente.
// La coppia (ingrediente,indice) verrà salvata in una hash table. l'array dinamico conterrà
// delle linked list di lotti.

typedef struct {
  Sezione * sez;
  size_t len;
} Magazzino;

typedef struct {
  Nome * ing;
  size_t len;
} Ingredienti;

typedef struct {
  Nome nome;
  Ricetta rc;
  int qnt;
  int peso;
  size_t t;
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
  size_t t;
  Ordine * buff;
  size_t len;
} Corriere;

/* TODO: ->
typedef struct{
  Arena arena;
  int cap;
  size_t t;
} Corriere;
*/ 

enum {
  AGG = 1,
  RMV,
  RIF,
  ORD,
  END,
} Istr; 

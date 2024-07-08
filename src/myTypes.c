#define STRMAXLEN 255 + 1

typedef struct Nodo{
  int val;
  struct Nodo * next;
} nodo_t;

typedef nodo_t * Ptr_nodo;

typedef char Nome[STRMAXLEN];

typedef struct Token{
  Nome tk; 
  struct Token * next;

} token_t;

typedef token_t * Ptr_token;

typedef struct {
  Ptr_token head;
  Ptr_token tail;
  size_t len;
} Input;

typedef struct {  
  int ingId;
  int qnt;
} CompRicetta;
 
typedef struct {
  Nome nome;
  CompRicetta * comp;
  size_t len;
} Ricetta;

typedef struct {
  Ricetta * rts;
  size_t len;
} Ricettario;

typedef struct Lotto{
  int qnt;
  int scadenza;
  struct Lotto * next;

} lotto_t;

typedef lotto_t * Ptr_lotto;

typedef struct {
  Ptr_lotto lt;
  Nome nomeIng;
  size_t ingId;
  size_t usedBy; // Numero di ricette che hanno come componente questo ingrediente
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
  //Nome nome;
  //TODO: ricontrolla che forse era meglio con la ricetta già salvata
  int rcId; //Id della ricetta
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

enum {
  AGG = 1,
  RMV,
  RIF,
  ORD,
  END,
} Istr; 

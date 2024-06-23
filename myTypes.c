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

// Il magazzino lo gestisco come un array dinamico a cui ogni idice corrisponde un Ingrediente.
// La coppia (ingrediente,indice) verrà salvata in una hash table. l'array dinamico conterrà
// delle linked list di lotti.

typedef struct {
  Ptr_lotto * sez;
  size_t len;
} Magazzino;

typedef struct {
  Nome * ing;
  size_t len;
} Ingredienti;

enum {
  AGG = 1,
  RMV,
  RIF,
  ORD,
  END,
} Istr; 

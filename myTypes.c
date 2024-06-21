#define STRMAXLEN 255 + 1

typedef char Nome[STRMAXLEN];

typedef struct {
  Nome * tokens;
  size_t dim;
} Input;

typedef struct{
  Nome nome;
} Ingrediente;

typedef struct {  
  Ingrediente ingr;
  int qnt;
} CompRicetta;
 
typedef struct {
  CompRicetta * comp;
  size_t qnt;
} Ricetta;

typedef struct {
  Ingrediente ingr;
  int qnt;
  int scadenza;
} Lotto;

typedef struct {
  Lotto * lt;
  int * scorte_id;
} Magazzino;

enum {
  AGG = 1,
  RMV,
  RIF,
  ORD,
  END,
} Istr; 

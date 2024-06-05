#define STRMAXLEN 255 + 1

typedef char Nome[STRMAXLEN];

typedef struct{
  int id; 
  Nome nome;
} Ingrediente;

typedef struct {  
  Ingrediente ingr;
  int qnt;
} CompRicetta;
 
typedef struct {
  CompRicetta * comp;
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
  AGG,
  RMV,
  RIF,
  ORD,
  END,
} Istr; 

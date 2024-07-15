typedef struct Cella{
  char * key;
  int id;

  struct Cella * p;
  struct Cella * left;
  struct Cella * right;

} cella_t;

typedef cella_t* Ptr_cella; 

typedef struct{
  Ptr_cella root;
} Albero;

Ptr_cella alloca_cella();
Ptr_cella init_cella(char * k, int id);
bool valore_minore(char * k1, char * k2);
Ptr_cella cerca_cella(Ptr_cella cl, char * k);
void aggiungi_cella(Albero * T, Ptr_cella elem);
Ptr_cella rimuovi_cella(Albero * T, Ptr_cella z);
void dealloca_albero(Ptr_cella x, void (*dealloca_dati) (int));
void stampa_albero(Ptr_cella x, int k);


#ifndef _MYTYP_H_
#define _MYTYP_H_

#include <stdbool.h>

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


#endif

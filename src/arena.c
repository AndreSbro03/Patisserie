#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "myTypes.c"

typedef struct {
  void * data;

  //size of data
  int sod;

  // Se il dato è un semplice array = 1
  // se è una matrice la cui dimensione delle righe è finita = dim righe
  // se è una matrice la cui dimensione delle righe è incognita bisognia "wrappare" le righe in uno struct in modo che 
  // non sia più una matrice ma un array di elementi di dimensioni finite.
  int offset; 
  size_t len;
} Arena;

//retunr old len
size_t expand_arena(Arena * a, size_t newLen){
  
  if(newLen - a->len <= 0) return a->len;
  
  size_t oldLen = a->len;

  a->data = realloc(a->data, newLen * a->sod);

  if(a->data == NULL){
    perror("Realloc failed!\n");
    exit(EXIT_FAILURE);
  }

  a->len = newLen;

  return oldLen;
}

// Inizializza tutti i nuovi slot con un determiato dato.
void init_arena_data(Arena * a, size_t oldLen, void * data){
  for(size_t i = oldLen; i < a->len; ++i){
    memcpy(a->data + i * a->offset, data, a->sod);
  }
}

void free_arena(Arena * a){
  free(a->data);
}

// aggiunge elemento alla fine dei dati
void append_arena_data(Arena * a, void * data){
  init_arena_data(a, expand_arena(a, a->len + 1), data);
}

/*
int main(){

  Arena magazzino = { .data = NULL, .sod = sizeof(Nome), .len = 0, .offset = sizeof(Nome) / sizeof(char)};

  printf("Arena: nome -> %s, len -> %ld\n", ((Nome *)magazzino.data)[0], magazzino.len);

  free_arena(&magazzino);

  return 0;
}
*/

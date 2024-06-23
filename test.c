#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef char String[256];

// Funzione di hash djb2
unsigned long hash_djb2(String str) {
    unsigned long hash = 5381;
    int c;

    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }

    return hash;
}
int main(){

  String c = "ciao";
  
  printf("%d\n", hash_djb2(c));

  return 0;
}

#include "algoritmi.h"
#include "myTypes.h"

//quicksort
void swap(Ordine * a, int i1, int i2){
    //printf("%d %d\n", i1, i2);
    Ordine t = a[i1];
    a[i1] = a[i2];
    a[i2] = t;
}

int partition(Ordine * a, int p, int r){

    Ordine x = a[r];
    int i = p - 1;

    for(int j = p; j < r; ++j){
      if(a[j].peso > x.peso){
        i = i + 1;
        swap(a, i, j);
      }
      else if(a[j].peso == x.peso){
        if(a[j].t <= x.t){
          i = i + 1;
          swap(a, i, j);
        }
      }
    }
    swap(a, i + 1, r);

    return i + 1;
}

void quicksort(Ordine * a, int p, int r){
    if(p < r){
        int q = partition(a, p, r);
        quicksort(a, p, q - 1);
        quicksort(a, q + 1, r);
    }
}

//insertion_sort
void insertion_sort(Ordine * a, int len){
  for(int j = 1; j < len; ++j){
    Ordine key = a[j];
    int i = (int) j - 1;

    while(i >= 0 && a[i].peso < key.peso){
      a[i + 1] = a[i];
      i--;
    }

    a[i + 1] = key;
  }
}


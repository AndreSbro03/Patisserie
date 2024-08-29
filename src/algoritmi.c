#include "algoritmi.h"
#include "myTypes.h"
#include <math.h>

//quicksort
void swap(lotto_t * a, int i1, int i2){
    //printf("%d %d\n", i1, i2);
    lotto_t t = a[i1];
    a[i1] = a[i2];
    a[i2] = t;
}

int partition(lotto_t * a, int p, int r){

  lotto_t x = a[r];
  int i = p - 1;
    
  for(int j = p; j < r; ++j){
    if(a[j].scadenza >= x.scadenza){
      i = i + 1;
      swap(a, i, j);
    }
  }
  
  swap(a, i + 1, r);

  return i + 1;
}

void quicksort(lotto_t * a, int p, int r){
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

void merge(Ptr_ordine arr[], int p, int q, int r, int type) 
{ 
  int i, j, k;

  int n1 = q - p + 1;
  int n2 = r - q;

  // Alloca due nuovi arrays
  Ptr_ordine L[n1], R[n2];

  for(i = 0; i < n1; ++i){
    L[i] = arr[p + i];
  }
  for(j = 0; j < n2; ++j){
    R[j] = arr[q + j + 1];
  }

  i = 0;
  j = 0;
  
  k = p;
  if(type == 1){ // Lavorando con tempo
    
    while (i < n1 && j < n2) { 
        if (L[i]->ord.t >= R[j]->ord.t) { 
            arr[k] = L[i]; 
            i++; 
        } 
        else { 
            arr[k] = R[j]; 
            j++; 
        } 
        k++; 
    } 

  }
  else if(type == 0){ // lavorando con peso
    
    while (i < n1 && j < n2) { 
        if (L[i]->ord.peso > R[j]->ord.peso) { 
            arr[k] = L[i]; 
            i++; 
        }
        else if( L[i]->ord.peso == R[j]->ord.peso) {
            if( L[i]->ord.t < R[j]->ord.t){
                arr[k] = L[i]; 
                i++; 
            }
            else{
                arr[k] = R[j]; 
            j++;
            }
        }
        else { 
            arr[k] = R[j]; 
            j++; 
        } 
        k++; 
    } 
  }
  

  // Copio i restanti elementi di L e di R
   while (i < n1) { 
      arr[k] = L[i]; 
      i++; 
      k++; 
  } 

  while (j < n2) { 
      arr[k] = R[j]; 
      j++; 
      k++; 
  } 
} 
  
void mergeSort(Ptr_ordine arr[], int p, int r, int type) 
{ 
    if (p < r) { 

      int q = (int) floor((p + r) / 2); 
  
      // Sort first and second halves 
      mergeSort(arr, p, q, type); 
      mergeSort(arr, q + 1, r, type); 
  
      merge(arr, p, q, r, type); 
    } 
} 

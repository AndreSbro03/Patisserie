#include "algoritmi.h"
#include "myTypes.h"

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

void merge_l(lotto_t arr[], int l, int m, int r) 
{ 
    int i, j, k; 
    int n1 = m - l + 1; 
    int n2 = r - m; 
  
    // Create temp arrays 
    lotto_t L[n1], R[n2]; 
  
    // Copy data to temp arrays 
    // L[] and R[] 
    for (i = 0; i < n1; i++) 
        L[i] = arr[l + i]; 
    for (j = 0; j < n2; j++) 
        R[j] = arr[m + 1 + j]; 
  
    // Merge the temp arrays back 
    // into arr[l..r] 
    // Initial index of first subarray 
    i = 0; 
  
    // Initial index of second subarray 
    j = 0; 
  
    // Initial index of merged subarray 
    k = l;
    
    while (i < n1 && j < n2) { 
        if (L[i].scadenza >= R[j].scadenza) { 
            arr[k] = L[i]; 
            i++; 
        } 
        else { 
            arr[k] = R[j]; 
            j++; 
        } 
        k++; 
    } 
  
    // Copy the remaining elements 
    // of L[], if there are any 
    while (i < n1) { 
        arr[k] = L[i]; 
        i++; 
        k++; 
    } 
  
    // Copy the remaining elements of 
    // R[], if there are any 
    while (j < n2) { 
        arr[k] = R[j]; 
        j++; 
        k++; 
    } 
} 
  
// l is for left index and r is 
// right index of the sub-array 
// of arr to be sorted 
void mergeSort_l(lotto_t arr[], int l, int r) 
{ 
    if (l < r) { 
        // Same as (l+r)/2, but avoids 
        // overflow for large l and r 
        int m = l + (r - l) / 2; 
  
        // Sort first and second halves 
        mergeSort_l(arr, l, m); 
        mergeSort_l(arr, m + 1, r); 
  
        merge_l(arr, l, m, r); 
    } 
} 

void merge(Ptr_ordine arr[], int l, int m, int r, int type) 
{ 
    int i, j, k; 
    int n1 = m - l + 1; 
    int n2 = r - m; 
  
    // Create temp arrays 
    Ptr_ordine L[n1], R[n2]; 
  
    // Copy data to temp arrays 
    // L[] and R[] 
    for (i = 0; i < n1; i++) 
        L[i] = arr[l + i]; 
    for (j = 0; j < n2; j++) 
        R[j] = arr[m + 1 + j]; 
  
    // Merge the temp arrays back 
    // into arr[l..r] 
    // Initial index of first subarray 
    i = 0; 
  
    // Initial index of second subarray 
    j = 0; 
  
    // Initial index of merged subarray 
    k = l;
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
  
    // Copy the remaining elements 
    // of L[], if there are any 
    while (i < n1) { 
        arr[k] = L[i]; 
        i++; 
        k++; 
    } 
  
    // Copy the remaining elements of 
    // R[], if there are any 
    while (j < n2) { 
        arr[k] = R[j]; 
        j++; 
        k++; 
    } 
} 
  
// l is for left index and r is 
// right index of the sub-array 
// of arr to be sorted 
void mergeSort(Ptr_ordine arr[], int l, int r, int type) 
{ 
    if (l < r) { 
        // Same as (l+r)/2, but avoids 
        // overflow for large l and r 
        int m = l + (r - l) / 2; 
  
        // Sort first and second halves 
        mergeSort(arr, l, m, type); 
        mergeSort(arr, m + 1, r, type); 
  
        merge(arr, l, m, r, type); 
    } 
} 

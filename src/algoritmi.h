#ifndef _MYALG_H_
#define _MYALG_H_

#include "myTypes.h"

#define WEIGHTS_DECR 0
#define TIME_CRESC 1

void quicksort(Ordine * a, int p, int r, int type);
void insertion_sort(Ordine * a, int len);
void mergeSort(Ordine arr[], int l, int r, int type); 

#endif

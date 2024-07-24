#ifndef _MYALG_H_
#define _MYALG_H_

#include "myTypes.h"

#define WEIGHTS_DECR 0
#define TIME_DECR 1

void quicksort(lotto_t * a, int p, int r);
void insertion_sort(Ordine * a, int len);
void mergeSort(Ptr_ordine arr[], int l, int r, int type);

#endif

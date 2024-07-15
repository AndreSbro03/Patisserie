#include <stdio.h>
#include <stdlib.h>

int size = 9;


void swap(int * a, int i1, int i2){
    //printf("%d %d\n", i1, i2);
    int t = a[i1];
    a[i1] = a[i2];
    a[i2] = t;
}

void printArr(int * a){
    for(int i = 0; i < size; ++i){
        printf("%d, ", a[i]);
    }
    printf("\n");
}

int partition(int * a, int p, int r){

    int x = a[r];
    int i = p - 1;

    for(int j = p; j < r; ++j){
        if(a[j] <= x){
            i = i + 1;
            swap(a, i, j);
        }
    }
    swap(a, i + 1, r);

    return i + 1;
}

void quicksort(int * a, int p, int r){
    if(p < r){
        int q = partition(a, p, r);
        quicksort(a, p, q - 1);
        quicksort(a, q + 1, r);
    }
}



int main(int argc, char *argv[]) {

    int * a = malloc(sizeof(int) * size);

    a[0] = 99;
    a[1] = 4;
    a[2] = 88;
    a[3] = 7;
    a[4] = 5;
    a[5] = -3;
    a[6] = 1;
    a[7] = 34;
    a[8] = 11;

    quicksort(a, 0, size - 1);

    printArr(a);
    free(a);

    return 0;
}

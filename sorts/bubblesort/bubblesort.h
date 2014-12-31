#ifndef __BUBBLESORT_H_
#define __BUBBLESORT_H_

#include <stdlib.h>

//void quickSwap(void **l, void **r);
//void quickSortPartition(void** array, int s, int e, int (*fcomp)(void*,void*));
void sort(void *data, size_t n, size_t s, int (*fcomp)(void*, void*));

#endif /* QUICKSORT_H_ */

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "../helpers.h"
#include "quicksort.h"


static void quickSortPartition(void* array, size_t size, int s, int e, int (*fcomp)(void*,void*))
{

  if((e - s) <= 0) return;

  int p = (e - s) / 2 + s;
  pswap(voidAdd(array, size, p), voidAdd(array, size, e), size);
  void *pval = voidAdd(array, size, e);

  int si = s;
  int i;
  for(i = s; i <= e-1; i++)
  {
    if(fcomp(voidAdd(array, size, i), pval) < 0)
    {
      pswap(voidAdd(array, size, si), voidAdd(array, size, i), size);
      si++;
    }
  }
  pswap(voidAdd(array, size, si), voidAdd(array, size, e), size);
  p = si;

  quickSortPartition(array, size, s, p-1, fcomp);
  quickSortPartition(array, size, p+1, e, fcomp);
}

void sort(void *data, size_t n, size_t s, int (*fcomp)(void*, void*))
{
  if(!data) return;
  if(n == 0) return;

  quickSortPartition(data, s, 0, n-1, fcomp);

  //return data;
}

char* getSortName(void)
{
  return "Quicksort";
}

char* getSortSymbol(void)
{
  return "sort";
}
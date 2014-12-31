#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "../helpers.h"
#include "bubblesort.h"

void sort(void *data, size_t n, size_t s, int (*fcomp)(void*, void*))
{
  if(!data) return;
  if(n == 0) return;

  size_t i, j;
  for(i = 0; i < n-1; i++)
  {
    for(j = i+1; j < n; j++)
    {
      if(fcomp(voidAdd(data, i, s), voidAdd(data, j, s)) > 0)
      {
        pswap(voidAdd(data, i, s), voidAdd(data, j, s), s);
      }
    }
  }

  //return data;
}

char* getSortName(void)
{
  return "Bubblesort";
}

char* getSortSymbol(void)
{
  return "sort";
}
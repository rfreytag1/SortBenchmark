#include <stdlib.h>
#include <string.h>
#include "helpers.h"

void* voidAdd(void *i, size_t size, ssize_t a)
{
  /*char *tmp = (char*)i;
  tmp = tmp + (a*size);
  return (void*)tmp;*/
  return i+(a*size);
}

unsigned long long totalSwaps = 0;

void pswap(void *l, void *r, size_t size)
{
  /*void tmp = *l;
  *l = *r;
  *r = tmp;*/
  if(l == r) return;
  totalSwaps++;
  void *tmp = malloc(size);
  memcpy(tmp, l, size);
  memcpy(l, r, size);
  memcpy(r, tmp, size);
  free(tmp);
}

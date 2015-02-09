/**
 * @file helpers.c
 * @date 18.12.2014
 * @author Roy Freytag
 *
 * some helper functions to be used in sorting algorithms
 */
#define _GNU_SOURCE

#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include "helpers.h"

/**
 * pointer arithmetic for generic pointers
 * @param i generic pointer
 * @param size size of a single element pointed to by i
 * @param a offset
 */
void* voidAdd(void *i, size_t size, ssize_t a)
{
  /*char *tmp = (char*)i;
  tmp = tmp + (a*size);
  return (void*)tmp;*/
  return i+(a*size);
}

unsigned long long totalSwaps = 0; ///< swap counter

#ifdef _GNU_SOURCE
static void* (*o_malloc)(size_t) = 0;
#endif
/**
 * swaps elements from l to r.
 *
 * both pointers need to point to of same size types
 * @param l left side
 * @param r right side
 * @param size size of the elements
 */
void pswap(void *l, void *r, size_t size)
{
#ifdef _GNU_SOURCE
  if(!o_malloc) o_malloc = dlsym(RTLD_NEXT, "malloc");
#endif
  /*void tmp = *l;
  *l = *r;
  *r = tmp;*/
  if(l == r) return;
  totalSwaps++;
#ifdef _GNU_SOURCE
  void *tmp = o_malloc(size);
#else
  void *tmp = malloc(size);
#endif
  memcpy(tmp, l, size);
  memcpy(l, r, size);
  memcpy(r, tmp, size);
  free(tmp);
}

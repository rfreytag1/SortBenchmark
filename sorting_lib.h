#ifndef __SORTING_LIB_H__
#define __SORTING_LIB_H__

typedef char* (*getSortNameFn_t)(void);
typedef char* (*getSortSymbolFn_t)(void);
typedef void** (*sortFn_t)(void*, size_t, size_t, int (*)(void*,void*));

#endif
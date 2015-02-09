#ifndef __SORTING_LIB_H__
#define __SORTING_LIB_H__
/**
 * @file sorting_lib.h
 * @author Roy Freytag
 * @brief Definitions for function-pointers required in the dynamically loaded modules
 *
 */

typedef char* (*getSortNameFn_t)(void); ///< Function-pointer type definition for Sort name getter
typedef char* (*getSortSymbolFn_t)(void); ///< Function-pointer type definition for Sort function symbol name getter
typedef void (*sortFn_t)(void*, size_t, size_t, int (*)(void*,void*)); ///< Function-pointer type definition for sort function, based on qsort

#endif

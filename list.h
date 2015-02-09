/**
 * @file list.h
 * @date 14.08.2012
 * @author Roy Freytag
 *
 * basic implementation of a doubly link list
 */

#ifndef __LIST_H__
#define __LIST_H__

/**
 * node struct for list
 */
typedef struct tListNode
{
  struct tListNode *pPrev; ///< pointer to previous node in list
  struct tListNode *pNext; ///< pointer to next node in list
  void *pData; ///< pointer to actuall data of this node
} ListCnct_t;

/**
 * list head
 */
typedef struct
{
  long iNodes; ///< count of nodes in list
  ListCnct_t *pFirst; ///< pointer to first element
  ListCnct_t *pLast; ///< pointer to last element
  ListCnct_t *pCur; ///< pointer to current element
} List_t;

List_t*   lst_createList();           //create list
int lst_deleteList(List_t *pList);        //delete the list from memory
int lst_deleteListData(List_t *pList, void (*fdata)(void*));  //delete the list from memory and the containing data

void*   lst_getFirst(List_t *pList);          //get first list element if available, else return a null pointer
void*   lst_getLast(List_t *pList);           //get last list element if ... ^
void* lst_getNext(List_t *pList);           //get next element in the list if ... ^
void* lst_getPrev(List_t *pList);           //get previous element in the list, if ... ^
void* lst_getSelected(List_t *pList);       //get current element in the list
void* lst_getIndexed(List_t *pList, int i);     //get element at position i

void*   lst_getMatching(List_t *pList, void* needle, int (*cmp)(void* a, void*b));

long  lst_getNodeCount(List_t *pList);        //get current count of elements

int lst_insertHead(List_t *pList, void *pData);     //add element as first element in list
int lst_insertTail(List_t *pList, void *pData);     //add element as last element in list
int lst_insertBehind(List_t *pList, void *pData);   //add element after current element
int lst_insertBefore(List_t *pList, void *pData);   //add element before current element
int lst_addItemToList(List_t *pList, void *pData, int (*cmp)(void*, void*));
                //add element at apropriate position in list(according to the compare function cmp)

void  lst_removeItem(List_t *pList);        //remove current element
void  lst_removeItemData(List_t *pList, void(*fdata)(void*)); //remove current element and its data
#endif

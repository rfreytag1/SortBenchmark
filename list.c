#include <stdlib.h>
#include <stdio.h>
#include "list.h"

List_t* lst_createList()
{
	List_t *pList;	
	pList = malloc(sizeof(List_t));
	if(!pList)
	{
		perror("malloc(createList)");
		return NULL;
	}
	//initialize values
	pList->iNodes = 0;
	pList->pFirst = NULL;
	pList->pLast = NULL;
	pList->pCur = NULL;
	//return pointer to the list
	return pList;
}

int lst_deleteList(List_t *pList)
{
	void* v;
	//iterate through the list and delete every node
	for(v=lst_getFirst(pList); v; v=lst_getSelected(pList))
	{
		lst_removeItem(pList);
	}	
	//delete the actual list
	free(pList);

	return 1;
}

int lst_deleteListData(List_t *pList, void (*fdata)(void*))
{
	void* v;
	//iterate through the list and delete every node
	for(v=lst_getFirst(pList); v; v=lst_getSelected(pList))
	{
		lst_removeItemData(pList, fdata);
	}	
	//delete the actual list
	free(pList);

	return 1;
}

void* lst_getFirst(List_t *pList)
{
	if(pList->pFirst)			//check if there's a first element
	{
		pList->pCur = pList->pFirst;	//set current element this element
		return pList->pCur->pData;	//return data pointer
	}
	else
	{
		return NULL;
	}
}

void* lst_getLast(List_t *pList)
{
	if(pList->pLast)
	{
		pList->pCur = pList->pLast;
		return pList->pCur->pData;
	}
	else
	{
		return NULL;
	}
}

void* lst_getNext(List_t *pList)
{
	if(pList->pCur)
	{
		if(pList->pCur->pNext)				//check if there's a next element
		{
			pList->pCur = pList->pCur->pNext;
			return pList->pCur->pData;
		}
		else
		{
			return NULL;
		}
	}	
	else
	{
		return NULL;
	}
}

void* lst_getPrev(List_t *pList)
{
	if(pList->pCur)
	{
		if(pList->pCur->pPrev)				//check if there's a previous element
		{
			pList->pCur = pList->pCur->pPrev;
			return pList->pCur->pData;
		}
		else
		{
			return NULL;
		}
	}
	else
	{
		return NULL;
	}
}

void* lst_getSelected(List_t *pList)
{
	if(pList->pCur)
	{
		return pList->pCur->pData;
	}
	else
	{
		return NULL;
	}
}

void* lst_getIndexed(List_t* pList, int idx)
{
	int i;
	
	for(i = 0, lst_getFirst(pList); i < idx && lst_getNext(pList); i++);
	
	if(i == idx)
	{
		return lst_getSelected(pList);
	}
	else
	{
		return NULL;
	}
}

long lst_getNodeCount(List_t *pList)
{
	return pList->iNodes;
}

int lst_insertHead(List_t *pList, void *pData)
{
	//create the element in memory
	ListCnct_t* tmp;
	tmp = malloc(sizeof(ListCnct_t));

	if(!tmp)
	{
		perror("malloc(insertHead)");
		return 0;
	}

	tmp->pData = pData;		//set the data pointer
	
	if(pList->pFirst)
	{
		tmp->pNext = pList->pFirst;	//add the current first element as next one of the new element
		pList->pFirst->pPrev = tmp;	//change the prev pointers adress to the new elements adress
	}
	else
	{
		tmp->pNext = NULL;
	}

	if(!pList->pLast)
	{
		pList->pLast = tmp;		//if there's no last element add the new one as last element as well
	}
	
	tmp->pPrev = NULL;			//there can't be a previous element to this one, so just set the pointer to NULL
	pList->pFirst = tmp;			//now add the new element as the first one in the list
	pList->pCur = tmp;			//also set it as current element
	pList->iNodes++;			//count up the node count
	return 1;
	//printf("pNext: %10p \t pPrev: %10p \t pData: %10p \t pMe: %10p\n", tmp->pNext, tmp->pPrev, tmp->pData, tmp);
}

int lst_insertTail(List_t *pList, void *pData)
{
	ListCnct_t *tmp;
	tmp = malloc(sizeof(ListCnct_t));

	if(!tmp)
	{
		perror("malloc(insertTail)");
		return 0;
	}

	tmp->pData = pData;

	if(pList->pLast)
	{
		tmp->pPrev = pList->pLast;	//add the current last element as previous element of the new element 
		pList->pLast->pNext = tmp;	//change the next pointers adress of the current element to the new elements adress		 
	}
	else
	{
		tmp->pPrev = NULL;
	}
	
	if(!pList->pFirst)
	{
		pList->pFirst = tmp;		//if there's no first element add this one as first as well
	}
	
	tmp->pNext = NULL;			//last element, so there can't be one after this, so set pointer to NULL
	pList->pLast = tmp;			//set it at last element
	pList->pCur = tmp;			//and also as current
	pList->iNodes++;			//count up node count
	return 1;
}

int lst_insertBefore(List_t *pList, void *pData)
{
	if(!pList->pCur)
	{
		return lst_insertHead(pList, pData);	//if there's no current elment there's none at all in the list, so add the new one as first
	}
	
	if(pList->pCur->pNext)
	{
		ListCnct_t *tmp;
		tmp = malloc(sizeof(ListCnct_t));

		if(!tmp)
		{
			perror("malloc(insertBefore)");
			return 0;
		}

		tmp->pData = pData;

		pList->pCur->pNext->pPrev = tmp;	//set the next elements pointer to the previous element to point to the new elements adress
		tmp->pNext = pList->pCur->pNext;	//set the next element of the new element to be the next element of the current element
		pList->pCur->pNext = tmp;		//set the next element of the old current element to be the new element
		tmp->pPrev = pList->pCur;		//also set the old current element as previous element of the new one 
		pList->pCur = tmp;			//set the new element as current one
		pList->iNodes++;		
		return 1;
	}
	else
	{
		return lst_insertTail(pList, pData);		//if the current element has no next element then just add the new one as last
	}
}

int lst_insertBehind(List_t *pList, void *pData)	//pretty much the same as insertBefore, only this time the new element is placed before the current one
{
	if(!pList->pCur)
	{
		return lst_insertHead(pList, pData);
	}

	if(pList->pCur->pPrev)
	{
		ListCnct_t *tmp;
		tmp = malloc(sizeof(ListCnct_t));

		if(!tmp)
		{
			perror("malloc(insertBehind)");
			return 0;
		}

		tmp->pData = pData;

		pList->pCur->pPrev->pNext = tmp;
		tmp->pPrev = pList->pCur->pPrev;
		pList->pCur->pPrev = tmp;
		tmp->pNext = pList->pCur;
		pList->pCur = tmp;
		pList->iNodes++;
		return 1;
	}
	else
	{
		return lst_insertHead(pList, pData);
	}
}

int lst_addItemToList(List_t *pList, void *pData, int (*cmp)(void*, void*))
{
	void *val;	//pointer to save the elements data pointer
	if(pList->iNodes == 0)
	{
		//if there are no nodes yet, add this one as first		
		return lst_insertHead(pList, pData);
	}

	//TODO: binary search here
	for(val = lst_getLast(pList); val; val = lst_getPrev(pList))	//iterate through the list from last to first element
	{
		if((*cmp)(val, pData))		//compare the 2 values with the given function
		{
			//add new element
			return lst_insertBefore(pList, pData);
		}
	}

	return lst_insertHead(pList, pData);	//if compare function never resulted in true, just add the new element as first one
}

void lst_removeItem(List_t *pList)
{
	if(pList->pCur)
	{
		if(pList->pCur == pList->pFirst)			//check if the current element is also the first
		{
			if(pList->pCur->pNext)				//check if there is an element after it
			{
				pList->pCur->pNext->pPrev = NULL;	//if yes, then set its prev pointer to NULL, because it'll be the new first element in the list
				pList->pFirst = pList->pCur->pNext;	//now set the element as stated before as first element
				free(pList->pCur);			//remove the old element from RAM
				pList->pCur = pList->pFirst;		//the current element is now the next element in the list, which is also the first element now		
			}
			else						//if it's the first element has no next element, then we can be sure the list is empty
			{
				free(pList->pCur);			//delete the element
				pList->pFirst = NULL;			//set everything in the list head to NULL, it's empty
				pList->pLast = NULL;
				pList->pCur = NULL;
			}

			pList->iNodes--;
			return;
		}
		else if(pList->pCur == pList->pLast)			//check if the current element is also the last one
		{
			if(pList->pCur->pPrev)				//check if it has an element before it
			{
				pList->pCur->pPrev->pNext = NULL;	//if yes make it the new last element
				pList->pLast = pList->pCur->pPrev;
				free(pList->pCur);
				pList->pCur = pList->pLast;
			}
			else						//if not, list will be empty after deleting this element
			{
				free(pList->pCur);
				pList->pFirst = NULL;
				pList->pLast = NULL;
				pList->pCur = NULL;
			}

			pList->iNodes--;
			return;
		}

		if(pList->pCur->pPrev)					//check if the current element has another element before it
		{
			pList->pCur->pPrev->pNext = pList->pCur->pNext;	//if yes, relink its next pointer to the element after the current one
		}

		if(pList->pCur->pNext)					//check if the current element has another element after it
		{
			pList->pCur->pNext->pPrev = pList->pCur->pPrev;	//if yes, relink its prev pointer to the element before the current one
		}
			
		free(pList->pCur);		//now delete the element
		pList->iNodes--;
	}
}

void lst_removeItemData(List_t *pList, void (*fdata)(void*))		//same as removeItem, but this time we also delete the containing data with the fdata function pointer
{
	if(pList->pCur)
	{
		if(pList->pCur == pList->pFirst)
		{
			if(pList->pCur->pNext)
			{
				pList->pCur->pNext->pPrev = NULL;
				pList->pFirst = pList->pCur->pNext;
				(*fdata)(pList->pCur->pData);
				free(pList->pCur);
				pList->pCur = pList->pFirst;
				
			}
			else
			{
				(*fdata)(pList->pCur->pData);
				free(pList->pCur);
				pList->pFirst = NULL;
				pList->pLast = NULL;
				pList->pCur = NULL;
			}

			pList->iNodes--;
			return;
		}
		else if(pList->pCur == pList->pLast)
		{
			if(pList->pCur->pPrev)
			{
				pList->pCur->pPrev->pNext = NULL;
				pList->pLast = pList->pCur->pPrev;
				(*fdata)(pList->pCur->pData);
				free(pList->pCur);
				pList->pCur = pList->pLast;
			}
			else
			{
				(*fdata)(pList->pCur->pData);
				free(pList->pCur);
				pList->pFirst = NULL;
				pList->pLast = NULL;
				pList->pCur = NULL;
			}

			pList->iNodes--;
			return;
		}

		if(pList->pCur->pPrev)
		{
			pList->pCur->pPrev->pNext = pList->pCur->pNext;
		}

		if(pList->pCur->pNext)
		{
			pList->pCur->pNext->pPrev = pList->pCur->pPrev;	
		}
			
		(*fdata)(pList->pCur->pData);
		free(pList->pCur);
		pList->iNodes--;
	}
}

void*   binSearch(List_t *pList, void* needle, int (*cmp)(void* a, void*b), int startidx, int endidx)
{
	void *data;
	if(startidx == endidx || endidx < 0)
	{
		data = lst_getIndexed(pList, startidx);
		if(data && cmp(data, needle) == 0)
			return data;
		else return NULL;
	}

	int middleidx = startidx + (endidx * 0.5) - 1;
	data = lst_getIndexed(pList, middleidx);
	int cres = cmp(data, needle);
	if(cres < 0)
	{
		return binSearch(pList, needle, cmp, startidx, middleidx-1);
	}
	else if(cres > 0)
	{
		return binSearch(pList, needle, cmp, middleidx+1, endidx);
	}
	else if(cres == 0)
	{
		return data;
	}

	return NULL;
}

void*   lst_getMatching(List_t *pList, void* needle, int (*cmp)(void* a, void*b))
{
	return binSearch(pList, needle, cmp, 0, lst_getNodeCount(pList) - 1);
}

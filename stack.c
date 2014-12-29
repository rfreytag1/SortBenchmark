/*
 * stack.c
 *
 *  Created on: 13.09.2014
 *      Author: pfannkuchen
 */

#include <stdio.h>
#include <stdlib.h>

#include "stack.h"

Stack_t *stk_createStack()
{
	Stack_t *tmp = malloc(sizeof(Stack_t));
	tmp->stackSize = 0;
	tmp->pFirst = 0;
	return tmp;
}

void stk_push(Stack_t *stack, void *data)
{
	StackElement_t *tmp = malloc(sizeof(StackElement_t));
	tmp->pNext = stack->pFirst;
	tmp->data = data;
	stack->pFirst = tmp;
	stack->stackSize++;
}

void *stk_pop(Stack_t *stack)
{
	if(!stack->stackSize) return NULL;
	StackElement_t *tmp = stack->pFirst;
	stack->pFirst = tmp->pNext;
	stack->stackSize--;
	void *data = tmp->data;
	free(tmp);
	return data;
}

void stk_destroyStack(Stack_t *stack)
{
	while(stack->stackSize) stk_pop(stack);
	free(stack);
}


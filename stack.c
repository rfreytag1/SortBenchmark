/**
 * @file stack.c
 *
 * @date 13.09.2014
 * @author Roy Freytag
 */

#include <stdio.h>
#include <stdlib.h>

#include "stack.h"

/**
 * @brief Creates an empty stack.
 * @return pointer to stack
 */
Stack_t *stk_createStack()
{
	Stack_t *tmp = malloc(sizeof(Stack_t));
	tmp->stackSize = 0;
	tmp->pFirst = 0;
	return tmp;
}

/**
 * @brief Pushes an Element onto the stack.
 * @param stack on which stack to put the data
 * @param data pointer to input data
 */
void stk_push(Stack_t *stack, void *data)
{
	StackElement_t *tmp = malloc(sizeof(StackElement_t));
	tmp->pNext = stack->pFirst;
	tmp->data = data;
	stack->pFirst = tmp;
	stack->stackSize++;
}

/**
 * @brief returns the top element of the stack and removes it.
 * @param stack which stack to pop from
 */
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

/**
 * @brief Removes all elements and frees the memory of the stack.
 * @param stack
 */
void stk_destroyStack(Stack_t *stack)
{
	while(stack->stackSize) stk_pop(stack);
	free(stack);
}


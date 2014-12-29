/*
 * stack.h
 *
 *  Created on: 13.09.2014
 *      Author: pfannkuchen
 */

#ifndef STACK_H_
#define STACK_H_

typedef struct StackElement
{
	struct StackElement *pNext;
	void *data;
} StackElement_t;

typedef struct
{
	StackElement_t *pFirst;
	unsigned long stackSize;
} Stack_t;

Stack_t *stk_createStack();
void stk_push(Stack_t *stack, void *data);
void *stk_pop(Stack_t *stack);
void stk_destroyStack(Stack_t *stack);


#endif /* STACK_H_ */

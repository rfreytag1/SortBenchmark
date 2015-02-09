/**
 * @file stack.h
 *
 * @date 13.09.2014
 * @author Roy Freytag
 */


#ifndef STACK_H_
#define STACK_H_

/**
 * @file stack.h
 * @brief a simple FILO Stack implementation
 */

/**
 * @brief Definition of the Container-element
 * which stores a pointer to the data and a pointer to the next element
 */
typedef struct StackElement
{
	struct StackElement *pNext; ///< Pointer to next Stack Element
	void *data; ///< Pointer to data
} StackElement_t;

/**
 * @brief Head of the stack
 *
 */
typedef struct Stack
{
	StackElement_t *pFirst; ///< Pointer to first Stack Element
	unsigned long stackSize; ///< contains the number of stack elements in this stack.
} Stack_t;

Stack_t *stk_createStack();
void stk_push(Stack_t *stack, void *data);
void *stk_pop(Stack_t *stack);
void stk_destroyStack(Stack_t *stack);


#endif /* STACK_H_ */

#ifndef _STACK_H
#define _STACK_H

/** Simple stack ADT */

/** Stack is a opaque structure. */
typedef struct StackImpl Stack;

/** A stack-entry is a generic pointer. */
typedef void *StackEntry;

/** All routines below return an int error-code: 0 if no error, otherwise
 *  a suitable errno code (errno(3)).
 */

/** Return pointer to a newly created empty stack. */
int new_stack(Stack **stackP);

/** Free all resources used by stack. */
int free_stack(Stack *stack);

/** Push entry onto stack. */
int push_stack(Stack *stack, StackEntry entry);

/** Pop top of stack into entryP. */
int pop_stack(Stack *stack, StackEntry *entryP);

/** Return number of entries currently on stack in *numEntryP. */
int size_stack(const Stack *stack, int *numEntryP);

/** Peek at stack entry with specified index (top of stack is index 0).
 *  Peek'd entry returned in *entryP.
 */
int peek_stack(const Stack *stack, int index, StackEntry *entryP);

#endif //ifndef _STACK_H

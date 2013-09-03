#include "stack.h"

#include <errno.h>
#include <stdlib.h>

struct StackImpl {
  int index;            /** index of next free entry */
  int size;             /** size of current entries */
  StackEntry *entries;  /** pointer to dynamically allocated entries */
};

/** All routines below return an int error-code: 0 if no error, otherwise
 *  a suitable errno code (errno(3)).
 */

/** Return pointer to a newly created empty stack. */
int
new_stack(Stack **stackP)
{
  Stack *stack = calloc(1, sizeof(Stack));
  if (stack == NULL) {
    return errno;
  }
  *stackP = stack;
  return 0;
}

/** Free all resources used by stack. */
int
free_stack(Stack *stack)
{
  free(stack->entries);
  free(stack);
  return 0;
}

/** Push entry onto stack. */
int
push_stack(Stack *stack, StackEntry entry)
{
  enum { STACK_INC = 16 };
  if (stack->index >= stack->size) {
    stack->entries = realloc(stack->entries,
                             (stack->size += STACK_INC)*sizeof(StackEntry));
    if (stack->entries == NULL) return errno;
  }
  stack->entries[stack->index++] = entry;
  return 0;
}

/** Pop top of stack into entryP. */
int
pop_stack(Stack *stack, StackEntry *entryP)
{
  if (stack->index <= 0) {
    return EFAULT;
  }
  *entryP = stack->entries[--stack->index];
  return 0;
}

/** Return number of entries currently on stack in *numEntryP. */
int
size_stack(const Stack *stack, int *numEntryP)
{
  *numEntryP = stack->index;
  return 0;
}

/** Peek at stack entry with specified index (top of stack is index 0).
 *  Peek'd entry returned in *entryP.
 */
int
peek_stack(const Stack *stack, int index, StackEntry *entryP)
{
  if (index < 0 || index >= stack->index) {
    return EFAULT;
  }
  *entryP = stack->entries[stack->index - 1 - index];
  return 0;
}

#if TEST_STACK
/** Simple test program: accepts int's from stdin interspersed with
 *  postfix operators:
 *
 *   integer::
 *     Push integer value on stack (integer may have - sign, but no + sign).
 *   '+'::
 *     Replace top 2 stack entries by their sum.
 *   '*'::
 *     Replace top 2 stack entries by their sum.
 *   '>'::
 *     Pop and print top stack entry.
 *   '='::
 *     Peek and print top stack entry.
 *   '#'::
 *     Peek and print all stack entries.
 *
 *  An example log follows:
 *
 *    $ ./test-stack
 *    2 3 * -15 + 20 #
 *    20
 *    -9
 *    =
 *    20
 *    -
 *    Bad message
 *    + #
 *    11
 *    $
 */

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

static inline void
error(int err)
{
  if (err != 0) fprintf(stderr, "%s\n", strerror(err));
}

static int add_op(int i1, int i2) { return i1 + i2; }
static int mul_op(int i1, int i2) { return i1 * i2; }

static int
add_mul(Stack *stack, int (*f)(int, int))
{
  StackEntry entry1, entry2;
  int err = pop_stack(stack, &entry1);
  if (err != 0) return err;
  if ((err = pop_stack(stack, &entry2)) != 0) return err;
  int i1 = (int)entry1, i2 = (int)entry2;
  err = push_stack(stack, (StackEntry)f(i1, i2));
  return err;
}

static int
push(Stack *stack, FILE *in)
{
  int v;
  if (fscanf(in, "%d", &v) != 1) {
    return EBADMSG;
  }
  int err = push_stack(stack, (StackEntry)v);
  return err;
}

static int
pop(Stack *stack, FILE *out)
{
  int i;
  int err = pop_stack(stack, (StackEntry *)&i);
  if (err != 0) {
    fprintf(out, "%d\n", i); fflush(out);
  }
  return err;
}

static int
peek(Stack *stack, int index, FILE *out)
{
  int size;
  int err = size_stack(stack, &size);
  if (err == 0) {
    int nPeeks = (index >= 0) ? 1 : size;
    for (int i = 0; err == 0 && i < nPeeks; i++) {
      int v;
      err = peek_stack(stack, i, (StackEntry)&v);
      if (err == 0) fprintf(out, "%d\n", v);
    }
    fflush(out);
  }
  return err;
}

static void
stack_ops(Stack *stack, FILE *in, FILE *out)
{
  int c;
  while ((c = fgetc(in)) != EOF) {
    int err;
    while (isspace(c)) { c = fgetc(in); }
    if (c == EOF) break;
    if (isdigit(c) || c == '-') {
      err = (ungetc(c, in) == c) ? push(stack, in) : EIO;
    }
    else {
      switch (c) {
      case '+':
        err = add_mul(stack, add_op);
        break;
      case '*':
        err = add_mul(stack, mul_op);
        break;
      case '>':
        err = pop(stack, out);
        break;
      case '=':
        err = peek(stack, 0, out);
        break;
      case '#':
        err = peek(stack, -1, out);
        break;
      default:
        err = EBADMSG;
      }
    } //else !isdigit(c)
    error(err);
  } //while (c != EOF);
}

int main() {
  Stack *stack;
  //make sure we can stuff a int into a StackEntry.
  assert(sizeof(int) <= sizeof(StackEntry));
  int err = new_stack(&stack);
  if (err == 0) {
    stack_ops(stack, stdin, stdout);
    err = free_stack(stack);
  }
  error(err);
  return 0;
}

#endif //if TEST_STACK

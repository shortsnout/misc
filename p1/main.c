#include <stdio.h>
#include "stack.h"
#include <assert.h>
#include <ctype.h>
#include <string.h>

int read_next_int(FILE *, int *);
#include "matrix.h"

static inline void
error(int err)
{
  if (err != 0) fprintf(stderr, "%s\n", strerror(err));
}

void debug(int x) {
  fprintf(stdout, "Info >> %d\n", x);
}

static int 
add_op(Matrix *m1, Matrix *m2, Matrix **out) 
{
  int m1rows, m2columns;
  int err = get_matrix_dimensions(m1, &m1rows, NULL);
  if(err != 0) return err;
  err = get_matrix_dimensions(m1, NULL, &m2columns);
  if(err != 0) return err;
  err = new_matrix(m1rows, m2columns, out);
  if(err != 0) return err;
  return add_matrix(m1, m2, *out);
}

static int 
mul_op(Matrix *m1, Matrix *m2, Matrix **out) 
{
  int m1rows, m2columns;
  int err = get_matrix_dimensions(m1, &m1rows, NULL);
  if(err != 0) return err;
  err = get_matrix_dimensions(m1, NULL, &m2columns);
  if(err != 0) return err;
  err = new_matrix(m1rows, m2columns, out);
  if(err != 0) return err;
  return multiply_matrix(m1, m2, *out);
}

static int
add_mul(Stack *stack, int (*f)(Matrix *, Matrix *, Matrix **))
{
  StackEntry entry1, entry2;
  int err = pop_stack(stack, &entry1);
  if (err != 0) return err;
  if ((err = pop_stack(stack, &entry2)) != 0) return err;
  Matrix *m1 = (Matrix *)entry1;
  Matrix *m2 = (Matrix *)entry2;
  Matrix **out = (Matrix **)malloc(sizeof(Matrix *));
  err = f(m1, m2, out);
  if (err != 0) return err;
  err = push_stack(stack, (StackEntry)*out);
  free_matrix(m1);
  free_matrix(m2);
  free(out);
  return err;
}

/*
 * Read the next int that appears in the input file stream
 */
int
read_next_int(FILE *in, int *value) {
  char c;
  if(value == NULL)
    return EINVAL;
  *value = 0;
  c = fgetc(in);
  while(isspace(c)){ c = fgetc(in); }
  while(isdigit(c)) {
    //to support multiple digit ints
    *value = *value * 10 + (c - '0');
    c = fgetc(in);
  }
  return 0;
}

static int
push(Stack *stack, FILE *in)
{
  int rows, columns, err;
  Matrix *oper = NULL;
  if (read_next_int(in, &rows) != 0) {
    return EBADMSG;
  }
  if (read_next_int(in, &columns) != 0) {
    return EBADMSG;
  }
  err = new_matrix(rows, columns, &oper);
  if(err != 0) return 0;
  err = read_matrix(in, oper);
  if(err != 0) return 0;
  err = push_stack(stack, (StackEntry)oper);
  return err;
}

static int
pop(Stack *stack, FILE *out)
{
  Matrix *i = NULL;
  int err = pop_stack(stack, (StackEntry *)i);
  if (err != 0) {
    fprintf(out, "%d\n", err); fflush(out);
  }
  free(i);
  return err;
}

static int
peek(Stack *stack, int index, FILE *out, int wipe)
{
  int size;
  int err = size_stack(stack, &size);
  if (err == 0) {
    int nPeeks = (index >= 0) ? 1 : size;
    for (int i = 0; err == 0 && i < nPeeks; i++) {
      Matrix **v = (Matrix **)malloc(sizeof(Matrix *));
      err = peek_stack(stack, i, (StackEntry)v);
      if (err == 0) {
          if(wipe == 1) free_matrix(*v); else write_matrix(out, *v);
      }
      free(v);
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
    if (c == EOF) 
      break;
    else {
      switch (c) {
      case '[':
        err = push(stack, in);
        break;
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
        err = peek(stack, 0, out, 0);
        break;
      case '#':
        err = peek(stack, -1, out, 0);
        break;
      default:
        err = EBADMSG;
      }
    } //else !isdigit(c)
    error(err);
  } //while (c != EOF);
  peek(stack, -1, stderr, 1);  //free up the stack
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

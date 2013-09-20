#include "matrix.h"

struct MatrixImpl {
  int rows;
  int columns;
  int *data;
};

/** Create a dynamically allocated uninitialized(thus malloc) int matrix
 *  having the specified number of rows and columns and set *matrix
 *  to point to the newly created matrix.
 */
int new_matrix(int numRows, int numCols, Matrix **matrix) {
    
  Matrix *matrixP = (Matrix *)malloc( sizeof(Matrix) );

  matrixP->data = (int *)malloc( numRows * numCols * sizeof(int) );
  if(matrixP == NULL) {
    return errno;
  }
  matrixP->rows = numRows;
  matrixP->columns = numCols;

  *matrix = matrixP;
  return 0;
}

/** Free all resources used by matrix. */
int free_matrix(Matrix *matrix) {
  free(matrix->data);
  free(matrix);
  return 0;
}

/** If numRowsP (numColsP) is not NULL, set what it points to to the
 *  number of rows (columns) in matrix.
 */
int get_matrix_dimensions(const Matrix *matrix, int *numRowsP, int *numColsP) { 
  if(numRowsP != NULL) {
    *numRowsP = matrix->rows;
  }
  if(numColsP != NULL) {
    *numColsP = matrix->columns;
  }
  return 0;
}

/** Set *value to matrix[i][j] (0-origin indexes).  Return suitable
 *  error if indexes are not valid 0-origin indexes for matrix.
 */
int get_matrix_entry(const Matrix *matrix, int i, int j, int *value) {
  if(i >= matrix->columns || i < 0) {
    return ECHRNG;
  }
  if(j >= matrix->rows || j < 0) {
    return ECHRNG;
  }
  if(value == NULL)
      value = (int *)malloc(sizeof(int));
  *value = matrix->data[ i * matrix->columns + j ];
  return 0;
}

/** Set entry for matrix[i][j] (0-origin indexes) to value.
 *  Return suitable error if indexes are not valid 0-origin indexes
 *  for matrix.
 */
int set_matrix_entry(Matrix *matrix, int i, int j, int value) {
  if(i >= matrix->columns || i < 0) {
    return ECHRNG;
  }
  if(j >= matrix->rows || j < 0) {
    return ECHRNG;
  }
  matrix->data[ i * matrix->columns + j ] = value;
  return 0;
}

/** Set entries of m x n matrix a to m x n whitespace separated int's
 *  read from text stream in in row-major format.
 */
int read_matrix(FILE *in, Matrix *a) {
  int rows, columns, value;
  rows = a->rows;
  columns = a->columns;
  for(int i = 0; i < rows; i++) {
    for(int j = 0; j < columns; j++) {
      if (read_next_int(in, &value) != 0) {
        return EINVAL;
      }
      set_matrix_entry(a, i, j, value);
    }
  }
  return 0;
}

/** Write entries of matrix a to text stream out with each row
 *  terminated by a '\n` and consecutive entries on a row separated
 *  using a single space character.  Write each entry using format
 *  "%8d".  Terminate the entire matrix with an additional newline
 *  (i.e., the last row is followed by 2 newlines).
 */
int write_matrix(FILE *out, const Matrix *a) { 
  if(out == NULL) {
    return EBADFD;
  }
  for(int i = 0; i < a->rows; ++i) {
    for(int j = 0; j < a->columns; ++j)
      fprintf(out, "%8d ", a->data[ i * a->columns + j ]);
    fprintf(out, "\n");
  }
  fprintf(out, "\n");
  return 0;
}

/** Set matrix c to the entry-by-entry sum of matrices a and b.
 *  Return suitable error if a and b are not compatible for addition.
 */
int add_matrix(const Matrix *a, const Matrix *b, Matrix *c) {
  if(a->rows != b->rows || a->columns != b->columns) {
    return EINVAL;
  }
  for(int i = 0; i < a->rows * a->columns; ++i) {
    c->data[i] = a->data[i] + b->data[i];
  }
  return 0;
}

/** Set matrix c to the matrix-product (not entry-by-entry product) of
 *  matrices a and b.  Return suitable error if a and b are not
 *  compatible for matrix multiplication.
 */
int multiply_matrix(const Matrix *a, const Matrix *b, Matrix *c) {
  if(a->columns != b->rows) {
    return EINVAL;
  }
  int columns = b->columns, rows = a->rows;
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < columns; ++j) {
      c->data[i * columns + j] = 0;
      for (int k = 0; k < columns; ++k) {
        c->data[i * columns + j] += a->data[i * columns + k] * b->data[k * columns + j];
      }
    }
  }
  /*
  for (int i = 0; i < rows * columns; ++i) {
    c->data[i] = 0;
    for (int k = 0; k < columns; ++k) {
      c->data[i] += a->data[i + k] * b->data[i + columns * k];
    }
  }
  */
  return 0;
}

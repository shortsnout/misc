#ifndef _MATRIX_H
#define _MATRIX_H

#include <stdio.h>

/** Integer matrix ADT */

/** All routines return 0 if there is no error; on error they return
 *  a value compatible with the documentation for errno (man errno(3)).
 */

/** Matrix is an opaque type referring to some MatrixImpl implementation
 *  structure representing an integer matrix.
 */
typedef struct MatrixImpl Matrix;

/** Create a dynamically allocated uninitialized int matrix
 *  having the specified number of rows and columns and set *matrix
 *  to point to the newly created matrix.
 */
int new_matrix(int numRows, int numCols, Matrix **matrix);

/** Free all resources used by matrix. */
int free_matrix(Matrix *matrix);

/** If numRowsP (numColsP) is not NULL, set what it points to to the
 *  number of rows (columns) in matrix.
 */
int get_matrix_dimensions(const Matrix *matrix, int *numRowsP, int *numColsP);

/** Set *value to matrix[i][j] (0-origin indexes).  Return suitable
 *  error if indexes are not valid 0-origin indexes for matrix.
 */
int get_matrix_entry(const Matrix *matrix, int i, int j, int *value);

/** Set entry for matrix[i][j] (0-origin indexes) to value.
 *  Return suitable error if indexes are not valid 0-origin indexes
 *  for matrix.
 */
int set_matrix_entry(Matrix *matrix, int i, int j, int value);

/** Set entries of m x n matrix a to m x n whitespace separated int's
 *  read from text stream in in row-major format.
 */
int read_matrix(FILE *in, Matrix *a);

/** Write entries of matrix a to text stream out with each row
 *  terminated by a '\n` and consecutive entries on a row separated
 *  using a single space character.  Write each entry using format
 *  "%8d".  Terminate the entire matrix with an additional newline
 *  (i.e., the last row is followed by 2 newlines).
 */
int write_matrix(FILE *out, const Matrix *a);

/** Set matrix c to the entry-by-entry sum of matrices a and b.
 *  Return suitable error if a and b are not compatible for addition.
 */
int add_matrix(const Matrix *a, const Matrix *b, Matrix *c);

/** Set matrix c to the matrix-product (not entry-by-entry product) of
 *  matrices a and b.  Return suitable error if a and b are not
 *  compatible for matrix multiplication.
 */
int multiply_matrix(const Matrix *a, const Matrix *b, Matrix *c);

#endif //#ifndef _MATRIX_H



/* All theese functions are void for avoiding memory leaks
 * when constructions "existed_matrix = foo()" are used, because
 * old array from lvalue stays in memory and only looses its ptr
 * after assigment.
 *
 * Function make_matrix is exception from this rule, because so stupid
 * use it on existing matrix. But this function isn't completely memory safety!
 * DON'T USE IT WITH EXISTING MATRIXES!!!!!
 *
 * ALWAYS INITIALIZE NEW MATRIXES BY FUNCTION make_matrix, it is wrong without initialization!!!!!!!!!!!!!!,
 * or initialize it by usual structure initialiser {0};
 *
 * */

#include <stdint.h>
#pragma once
#define IN
#define OUT
#define MODYFIED


typedef struct{
  int n_rows;
  int n_columns;
  double **array;
}matrix;


typedef double(*init_user)(int, int);

matrix make_matrix(const int n_rows, const int n_columns); // Do not use it on existing matrixes!!!

double determinant(const matrix*);

int copy_matrix(IN const matrix*, OUT matrix*);
int transponent(IN const matrix*, OUT matrix*);
int multiplex_matrixes(IN const matrix*, IN const matrix*, OUT matrix*);
int triangle_form(IN const matrix*, OUT matrix*);

int show_matrix(IN const matrix*);
int init_matrix(OUT matrix*);
int init_matrix_by_random(OUT matrix*,  const int32_t down, const int32_t up);
int init_matrix_as_unit(OUT matrix*);
int init_matrix_by_function(OUT matrix*, init_user);

int delete_matrix(matrix*);

int rows_swap(const int i_row_1, const int i_row_2, MODYFIED matrix*);
int columns_swap(const int i_col_1, const int i_col_2, MODYFIED matrix*);

int row_mult_on_const(const double factor, const int i_row, MODYFIED matrix*);
int column_mult_on_const(const double factor, const int i_column, MODYFIED matrix*);

int rows_sub(const double factor, const int i_subtracted_row, const int i_subtracting_row, MODYFIED matrix*);
int columns_sub(const double factor, const int i_subtracted_column, const int i_subtracting_column, MODYFIED matrix*);

int copy_row_to_other_matrix(const int i_source, const int i_reciever, IN const matrix *in_matrix, OUT matrix *out_matrix);
int copy_column_to_other_matrix(const int i_source, const int i_reciever, IN const matrix *in_matrix, OUT matrix *out_matrix);

int inverse_matrix(IN const matrix *in_matrix, OUT matrix *out_matrix);


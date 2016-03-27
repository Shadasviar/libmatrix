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

#pragma once
#define IN
#define OUT
#define MODYFIED

typedef struct{
  int n_rows;
  int n_columns;
  double **array;
}matrix;

matrix make_matrix(const int n_rows, const int n_columns); // Do not use it on existing matrixes!!!

double determinant(IN matrix*);

void copy_matrix(IN matrix*, OUT matrix*);
void transponent(IN matrix*, OUT matrix*);
void multiplex_matrixes(IN matrix*, IN matrix*, OUT matrix*);
void triangle_form(IN matrix*, OUT matrix*); //is it needed in the interface? REPROJECT IT!!!!!!!!!!!

void show_matrix(IN matrix*);
void init_matrix(OUT matrix*);
void init_matrix_by_random(OUT matrix*);
void init_matrix_as_unit(OUT matrix*);

void delete_matrix(matrix*);

void rows_swap(const int i_row_1, const int i_row_2, MODYFIED matrix*);
void columns_swap(const int i_col_1, const int i_col_2, MODYFIED matrix*);

void row_mult_on_const(const double factor, const int i_row, MODYFIED matrix*);
void column_mult_on_const(const double factor, const int i_column, MODYFIED matrix*);

void rows_sub(const double factor, const int i_subtracted_row, const int i_subtracting_row, MODYFIED matrix*);
void columns_sub(const double factor, const int i_subtracted_column, const int i_subtracting_column, MODYFIED matrix*);

void copy_row_to_other_matrix(const int i_source, const int i_reciever, IN matrix*, OUT matrix* );
void copy_column_to_other_matrix(const int i_source, const int i_reciever, IN matrix*, OUT matrix* );

void inverse_matrix(IN matrix*, OUT matrix*);


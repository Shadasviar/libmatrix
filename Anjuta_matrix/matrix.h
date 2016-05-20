/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*-  */
/*
 * matrix.h
 * Copyright (C) 2016 
 * 
 * libmatrix is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * libmatrix is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.";
 */


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
#define MODIFIED


typedef struct{
  int n_rows;
  int n_columns;
  double **array;
  int n_permutations;
}matrix;


typedef double(*init_user)(int, int);

/*Do not use it on existing matrixes!!!*/
matrix make_matrix(int n_rows, int n_columns); 

double determinant(IN const matrix*);
int rank(IN const matrix*);

int copy_matrix(IN const matrix*, OUT matrix*);
int transpose(IN const matrix*, OUT matrix*);
int multiplex_matrices(IN const matrix*,IN const matrix*, OUT matrix*);
int triangle_form(IN const matrix*, OUT matrix*);

int show_matrix(IN const matrix*);
int init_matrix(MODIFIED matrix*);
int init_matrix_by_random(MODIFIED matrix*, int down, int up);
int init_matrix_as_unit(MODIFIED matrix*);
int init_matrix_by_function(MODIFIED matrix*, init_user);

int delete_matrix(matrix*);

int rows_swap(int i_row_1, int i_row_2, MODIFIED matrix*);
int columns_swap(int i_col_1, int i_col_2, MODIFIED matrix*);

int row_mult_on_const(double factor, int i_row, MODIFIED matrix*);
int column_mult_on_const(double factor, int i_column, MODIFIED matrix*);

int rows_sub(double factor, int i_subtracted_row, int i_subtracting_row, MODIFIED matrix*);
int columns_sub(double factor, int i_subtracted_column, int i_subtracting_column, MODIFIED matrix*);

int copy_row_to_other_matrix(int i_source, int i_receiver, IN const matrix*, OUT matrix*);
int copy_column_to_other_matrix(int i_source, int i_receiver, IN const matrix*, OUT matrix*);

int inverse_matrix(IN const matrix*, OUT matrix*);


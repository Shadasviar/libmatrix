/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*-  */
/*
 * matrix.c
 * Copyright (C) 2016 
 * 
 * This file is part of libmatrix. 
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

#include "matrix.h"
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

#define UNUSED 0
#define GENERAL_ERR "WARNING: Check your input data. Something gone wrong"
#define NOT_ENOUGH_MEMORY_EXIT {puts("make_matrix: ERROR: NOT ENOUGH MEMORY"); exit(1);}

/* This structure is only for incapsulating of 
 * three parameters in one. Also it removes
 * compiler warnings about unused vars.
 */ 
typedef struct{
  IN const matrix *in_matrix;
  OUT matrix *out_matrix;
  void *param;
}iomatr;

typedef void (*action)(int, int, iomatr);

/*This function transmitts own parameters to
 *walk_on_matrix function if call it in the 
 *parameters list of walk_on_matrix. 
 */
iomatr transmit_params(IN const matrix*, OUT matrix*, void* );

/*Do action with every element of matrices in
 * the iomatr
 */
inline int walk_on_matrix(iomatr, action);

inline void show(int, int, iomatr);
inline void init(int, int, iomatr);
inline void init_by_random(int, int, iomatr);
inline void init_as_unit(int, int, iomatr);
inline void init_by_foo(int, int, iomatr);
inline void copy_element(int, int, iomatr);
inline void transpon(int, int, iomatr);
inline void add(int, int, iomatr);
inline void sub(int, int, iomatr);


int matrix_exists(const matrix *p_matrix);
int indexes_are_right(int index_1, int index_2, int max_val);
int triangle_form_of_augmented_matrix(IN const matrix*, OUT matrix*, OUT matrix*);

/* ---------------------------------------------------------------------------------------------------------------
 *Begin of realisation of the interface
 *
 * */


matrix make_matrix(int n_rows, int n_columns){

  double **array = (double**) calloc(n_rows, sizeof(*array));
  if(array == NULL) NOT_ENOUGH_MEMORY_EXIT;
  for(int i = 0; i < n_rows; i++){
    array[i] = (double*) calloc(n_columns, sizeof(*array[i]));
    if(array[i] != NULL){
      memset(array[i], 0.0, n_columns * sizeof(*array[i]));
    }
    else NOT_ENOUGH_MEMORY_EXIT;
  }

  matrix result = {0};
  result.n_rows = n_rows;
  result.n_columns = n_columns;
  result.array = array;
  return result;
}


int show_matrix(IN const matrix *in_matrix){
  return walk_on_matrix(transmit_params(in_matrix,UNUSED, UNUSED), show);
}


int init_matrix(MODIFIED matrix *out_matrix){
  return walk_on_matrix(transmit_params(UNUSED, out_matrix, UNUSED), init);
}


int init_matrix_by_random(MODIFIED matrix *out_matrix, int down, int up){ 
  int param[2] = {down, up};
  return walk_on_matrix(transmit_params(UNUSED, out_matrix, param), init_by_random);
}


int init_matrix_as_unit(MODIFIED matrix *out_matrix){
  return walk_on_matrix(transmit_params(UNUSED, out_matrix, UNUSED), init_as_unit);
}


int init_matrix_by_function(MODIFIED matrix *in_matrix, init_user foo){
 return walk_on_matrix(transmit_params(UNUSED, in_matrix, foo), init_by_foo);
}


int transpose(IN const matrix *in_matrix, OUT matrix *out_matrix){
  int status = true;  
  matrix result = make_matrix(in_matrix->n_columns, in_matrix->n_rows);
  
  status = status && walk_on_matrix(transmit_params(in_matrix, &result, UNUSED), transpon);
  status = status && copy_matrix(&result, out_matrix);
  
  delete_matrix(&result);
  
  return status;
}


int multiplex_matrices(IN const matrix *in_matrix_1, IN const matrix *in_matrix_2, OUT matrix *out_matrix){
  
  if(in_matrix_2->n_rows == in_matrix_1->n_columns){
    
    matrix result = make_matrix(in_matrix_1->n_rows, in_matrix_2->n_columns);

    for(int i = 0; i < result.n_rows ; i++){
      for(int j = 0; j < result.n_columns; j++){
        for(int k = 0; k < in_matrix_1->n_columns; k++){
          result.array[i][j] += in_matrix_1->array[i][k] * in_matrix_2->array[k][j];
        }
      }
    }
    
    copy_matrix(&result, out_matrix);
    delete_matrix(&result);
    return true;
  }
  
  return false;
}


double determinant(IN const matrix *in_matrix){
  
  if(matrix_exists(in_matrix) && in_matrix->n_rows == in_matrix->n_columns){    
    
    double result = 1.0;
    matrix triangle = make_matrix(0,0);
    triangle_form(in_matrix, &triangle);

    for(int i = 0; i < in_matrix->n_rows; i++){
      result *= triangle.array[i][i];
    }
    
    result *= pow(-1, triangle.n_permutations);
    delete_matrix(&triangle);
    
    return result;
  }
  else{
    puts("determinant: " GENERAL_ERR);
    return 0;
  }
}


int rank(IN const matrix *in_matrix){
  
  if(matrix_exists(in_matrix)){
    
    matrix tmp = make_matrix(0,0);
    copy_matrix(in_matrix, &tmp);
    triangle_form(&tmp, &tmp);
    
    int result = tmp.n_rows;
    
    for(int i = 0; i < tmp.n_rows; ++i){
      for(int j = tmp.n_columns-1; j >= 0; --j){
        
        if(tmp.array[i][j] != 0) break;
        if(j == 0) --result;
      }
    }
    
    delete_matrix(&tmp);
    return result;
  }
  
  return 0;
}


int copy_matrix(IN const matrix *in_matrix, OUT matrix *out_matrix){
  
  if(in_matrix != out_matrix
     && matrix_exists(out_matrix)
     && matrix_exists(in_matrix))
  {
    int status = true;
    delete_matrix(out_matrix);
    
    *out_matrix = make_matrix(in_matrix->n_rows, in_matrix->n_columns);
    status = status && walk_on_matrix(transmit_params(in_matrix, out_matrix, UNUSED), copy_element);
    out_matrix->n_permutations = in_matrix->n_permutations;
    
    return status;
  }
  
  return false;
}


int delete_matrix(matrix *in_matrix){
  
  if(matrix_exists(in_matrix)){
    
    for(int i = 0; i < in_matrix->n_rows; i++){
      
      if(in_matrix->array[i]){
        
        free(in_matrix->array[i]);
        in_matrix->array[i] = NULL;
      }
    }
    
    free(in_matrix->array);
    in_matrix->array = NULL;
    return true;
  }
  
  return false;
}


int rows_swap(int i_row_1, int i_row_2, MODIFIED matrix *in_matrix){
  
  if(indexes_are_right(i_row_1, i_row_2, in_matrix->n_rows)){
    
    double buf = 0;
    for(int i = 0; i < in_matrix->n_columns; i++){
      buf = in_matrix->array[i_row_1][i];
      in_matrix->array[i_row_1][i] = in_matrix->array[i_row_2][i];
      in_matrix->array[i_row_2][i] = buf;
    }
    
    ++(in_matrix->n_permutations);
    return true;
  }
  
  return false;
}


int columns_swap(int i_col_1, int i_col_2, MODIFIED matrix *in_matrix){
  
  int status = true;
  
  status = status && transpose(in_matrix, in_matrix);
  status = status && rows_swap(i_col_1, i_col_2, in_matrix);
  status = status && transpose(in_matrix, in_matrix);
  
  return status;
}


int triangle_form(IN const matrix *in_matrix, OUT matrix *out_matrix){
  return triangle_form_of_augmented_matrix(in_matrix, out_matrix, NULL);
}


int row_mult_on_const(double factor, int i_row, MODIFIED matrix *out_matrix){
  
  if(indexes_are_right(i_row, UNUSED, out_matrix->n_rows) 
     && matrix_exists(out_matrix))
  {
    
    for(int i = 0; i < out_matrix->n_columns; i++){
      out_matrix->array[i_row][i] *= factor;
    }
    return true;
  }

  return false;
}


int column_mult_on_const(double factor, int i_column, MODIFIED matrix *out_matrix){
  
  int status = true;
  
  status = status && transpose(out_matrix, out_matrix);
  status = status && row_mult_on_const(factor, i_column, out_matrix);
  status = status && transpose(out_matrix, out_matrix);
  
  return status;
}


int rows_sub(
  double factor,
  int i_subtracted_row,
  int i_subtracting_row,
  MODIFIED matrix *out_matrix)
{
  if(indexes_are_right(i_subtracted_row, i_subtracting_row, out_matrix->n_rows) 
     && matrix_exists(out_matrix))
  {
    
    for(int i = 0; i < out_matrix->n_columns; i++){
      out_matrix->array[i_subtracted_row][i] -= out_matrix->array[i_subtracting_row][i] * factor;
    }
    return true;
  }

  return false;
}


int columns_sub(
  double factor,
  int i_subtracted_column,
  int i_subtracting_column,
  MODIFIED matrix *out_matrix)
{
  int status = true;
  
  status = status && transpose(out_matrix, out_matrix);
  status = status && rows_sub(factor, i_subtracted_column, i_subtracting_column, out_matrix);
  status = status && transpose(out_matrix, out_matrix);
  
  return status;
}


int copy_row_to_other_matrix(
  int i_source,
  int i_receiver,
  IN const matrix *in_matrix,
  OUT matrix *out_matrix )
{
  
  if(matrix_exists(in_matrix) 
    && matrix_exists(out_matrix)
    && indexes_are_right(i_source, UNUSED, in_matrix->n_rows)
    && indexes_are_right(i_receiver, UNUSED, out_matrix->n_rows)
    && in_matrix->n_columns == out_matrix->n_columns)
  {
    
    for(int i = 0; i < in_matrix->n_columns; i++){
      out_matrix->array[i_receiver][i] = in_matrix->array[i_source][i];
    }
    return true;
  }

  return false;
}


int copy_column_to_other_matrix(
  int i_source,
  int i_receiver,
  IN const matrix *in_matrix,
  OUT matrix *out_matrix )
{
  int status = true;
  matrix tmp = make_matrix(0,0);
  
  status = status && transpose(in_matrix, &tmp);
  status = status && transpose(out_matrix, out_matrix);
  
  status = status && copy_row_to_other_matrix(i_source, i_receiver, &tmp, out_matrix);
  
  status = status && transpose(out_matrix, out_matrix);
  status = status && delete_matrix(&tmp);
  
  return status;
}


int add_matrices(IN const matrix *in_matrix_1, IN const matrix *in_matrix_2, OUT matrix *out_matrix){
 if(matrix_exists(in_matrix_1)
     && matrix_exists(in_matrix_2)
     && matrix_exists(out_matrix)
     &&in_matrix_1->n_rows == in_matrix_2->n_rows
     && in_matrix_1->n_columns == in_matrix_2->n_columns)
  {
    int status = true;   
    status = status && delete_matrix(out_matrix);
    *out_matrix= make_matrix(in_matrix_2->n_rows, in_matrix_1->n_columns);

    /* This complicated call is for transmitting two const params without warnings*/
    iomatr tmp = transmit_params(in_matrix_1, UNUSED, UNUSED);
    status = status && walk_on_matrix(transmit_params(in_matrix_1, out_matrix, &tmp), add);
    return status;
  }
  
  return false;
}


int sub_matrices(IN const matrix *in_matrix_1, IN const matrix *in_matrix_2, OUT matrix *out_matrix){
  if(matrix_exists(in_matrix_1)
     && matrix_exists(in_matrix_2)
     && matrix_exists(out_matrix)
     &&in_matrix_1->n_rows == in_matrix_2->n_rows
     && in_matrix_1->n_columns == in_matrix_2->n_columns)
  {
    int status = true;   
    status = status && delete_matrix(out_matrix);
    *out_matrix= make_matrix(in_matrix_1->n_rows, in_matrix_1->n_columns);

    /* This complicated call is for transmitting two const params without warnings*/
    iomatr tmp = transmit_params(in_matrix_2, UNUSED, UNUSED);
    status = status && walk_on_matrix(transmit_params(in_matrix_1, out_matrix, &tmp), sub);
    return status;
  }
  
  return false;
}


int inverse_matrix(IN const matrix *in_matrix, OUT matrix *out_matrix){
  
  if(matrix_exists(in_matrix)
     && in_matrix->n_columns == in_matrix->n_rows
     && determinant(in_matrix) != 0)
  {
    int status = true;
		
    matrix result = make_matrix(in_matrix->n_rows, in_matrix->n_columns);
    matrix tmp = make_matrix(0,0);    
    status = status && init_matrix_as_unit(&result);
    status = status && copy_matrix(in_matrix, &tmp);

    /* Forward substitution*/
    status = status && triangle_form_of_augmented_matrix(&tmp, &tmp, &result);

    /* Back substitution*/
    for(int i = tmp.n_rows-1; i > 0; i--){
      
      double factor = tmp.array[i][i];
      status = status && row_mult_on_const(1/factor, i, &tmp);
      status = status && row_mult_on_const(1/factor, i, &result);
      
      for(int j = 0; j < i; j++){        
        factor = tmp.array[j][i];
        status = status && rows_sub(factor, j, i, &tmp);
        status = status && rows_sub(factor, j, i, &result);
      }
    }
    
    status = status && copy_matrix(&result, out_matrix);
    status = status && delete_matrix(&result);
    status = status && delete_matrix(&tmp);
    return status;
  }

  return false;
}


/*
 * End of realisation of the interface
 *--------------------------------------------------------------------------------------------------------------
 * */

int matrix_exists(const matrix *p_matrix){
  return (p_matrix && p_matrix->array);
}


int indexes_are_right(int index_1, int index_2, int max_val){
  return (index_1 >= 0 && index_1 < max_val) && (index_2 >= 0 && index_2 < max_val);
}


int triangle_form_of_augmented_matrix(
  IN const matrix *in_matrix,
  OUT matrix *out_matrix,
  OUT matrix *half_an_inverted_matrix)
{
  int status = true;
  matrix result = make_matrix(0,0);
  matrix result_2 = make_matrix(in_matrix->n_rows,in_matrix->n_columns);
  
  status = status && copy_matrix(in_matrix, &result);
  status = status && init_matrix_as_unit(&result_2);

  for(int i = 0; i < result.n_columns; i++){

    /* This loop swaps current row with other one from next rows if element on the 
     * diagonal is zero. If all elements after diagonal have zeros too, determinant is zero 
     * because rank of matrix is less than number of its rows or columns. 
     */
    for(int k = i+1; k < result.n_rows; k++){
      if(result.array[i][i] != 0){
        break;
      }
      else{
        status = status && rows_swap(i, k, &result);
        status = status && rows_swap(i, k, &result_2);
      }
    }
		
    for(int j = 1+i; j < result.n_rows; j++){
			
      if(result.array[i][i] != 0 && result.array[j][i] != 0){    
        
        double tmp = result.array[j][i];
        
        status = status && row_mult_on_const(result.array[i][i], j, &result);
        status = status && row_mult_on_const(result.array[i][i], j, &result_2);
        
        status = status && rows_sub(tmp, j, i, &result);
        status = status && rows_sub(tmp, j, i, &result_2);
        
        status = status && row_mult_on_const(1/result.array[i][i], j, &result);
        status = status && row_mult_on_const(1/result.array[i][i], j, &result_2);
      }
      else continue;
    }
  }

  status = status && copy_matrix(&result, out_matrix);
  status = status && copy_matrix(&result_2, half_an_inverted_matrix);
  
  status = status && delete_matrix(&result);
  status = status && delete_matrix(&result_2);
  
  return status;
}


int walk_on_matrix(iomatr matr, action foo){
  
  if(matrix_exists(matr.in_matrix) || matrix_exists(matr.out_matrix)){
    
    int n_rows = matrix_exists(matr.in_matrix) 
      ? matr.in_matrix->n_rows 
      : matr.out_matrix->n_rows;
    
    int n_columns = matrix_exists(matr.in_matrix) 
      ? matr.in_matrix->n_columns 
      : matr.out_matrix->n_columns;
    
    for(int i = 0; i < n_rows; i++){
      for(int j = 0; j < n_columns; j++){
        foo(i, j, matr);
      }
    }
    return true;
  }

  return false;
}


void init_by_random(int i_row, int i_column, iomatr matr){
  
  srand((int)time(NULL)+(rand()));
  
  int down = ((int*)matr.param)[0];
  int up = ((int*)matr.param)[1];
  
  matr.out_matrix->array[i_row][i_column] = ((rand() % (up - down + 1)) + down);
}


void show(int i_row, int i_column, iomatr matr){
  printf(" %.2f ", matr.in_matrix->array[i_row][i_column]);
  if(i_column == matr.in_matrix->n_columns-1) puts("");
}


void init(int i_row, int i_column, iomatr matr){
  printf("Enter %d %d elment: ",i_row, i_column);
  scanf("%lf", &matr.out_matrix->array[i_row][i_column]);
}


void init_as_unit(int i_row, int i_column, iomatr matr){
  if(i_row == i_column) matr.out_matrix->array[i_row][i_column] = 1;
}


void init_by_foo(int i_row, int i_column, iomatr matr){
  init_user foo = (init_user)matr.param;
  matr.out_matrix->array[i_row][i_column] = foo(i_row, i_column);
}


void copy_element(int i_row, int i_column, iomatr matr){
  matr.out_matrix->array[i_row][i_column] = matr.in_matrix->array[i_row][i_column];
}


void transpon(int i_row, int i_column, iomatr matr){
  matr.out_matrix->array[i_column][i_row] = matr.in_matrix->array[i_row][i_column];
}


iomatr transmit_params(const matrix *in_matrix, matrix *out_matrix, void *param){
  return (iomatr) { in_matrix, out_matrix, param};
}


void add(int i_row, int i_column, iomatr matr){
  matr.out_matrix->array[i_row][i_column] =
    matr.in_matrix->array[i_row][i_column] + ((iomatr*)(matr.param))->in_matrix->array[i_row][i_column];
}


void sub(int i_row, int i_column, iomatr matr){
  matr.out_matrix->array[i_row][i_column] =
    matr.in_matrix->array[i_row][i_column] - ((iomatr*)(matr.param))->in_matrix->array[i_row][i_column];
}


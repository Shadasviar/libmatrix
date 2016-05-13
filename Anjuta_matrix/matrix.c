#include "matrix.h"
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

/* WORD means double's size */
#define FIRST_HALF_OF_WORD 0xFFFFFFFF00000000
#define SECOND_HALF_OF_WORD 0x00000000FFFFFFFF
#define FULL_WORD 0xFFFFFFFFFFFFFFFF
#define LENGHT_OF_WORD 64

#define UNUSED
#define UNUSED_PARAM 0
#define UNUSED_F(x) (void)(x)
#define GENERAL_ERR "WARNING: Check your input data. Something gone wrong"
#define NOT_ENOUGH_MEMORY_EXIT {puts("make_matrix: ERROR: NOT ENOUGH MEMORY"); exit(1);}

typedef void (*action)(int, int, const IN matrix*, OUT matrix*, double);


int walk_on_matrix(const IN matrix*, OUT matrix*, action, double);

inline void show(int, int, const IN matrix*, UNUSED matrix*, UNUSED double);
inline void init(int, int, UNUSED const matrix*, OUT matrix*, UNUSED double);
inline void init_by_random(int, int, UNUSED const matrix*, OUT matrix*, double);
inline void init_as_unit(int, int, UNUSED const matrix*, OUT matrix*, UNUSED double);
inline void mult(int, int, UNUSED const matrix*, OUT matrix*, double);
inline void copy_element(int, int, const IN matrix*, OUT matrix*, UNUSED double);
inline void transpon(int, int, const IN matrix*, OUT matrix*, UNUSED double);


int matrix_exists(const matrix *p_matrix);
int indexes_are_right(int index_1, int index_2, int max_val);
int triangle_form_of_augmented_matrix(IN const matrix*, OUT matrix*, OUT matrix*);

/* ---------------------------------------------------------------------------------------------------------------
 *Begin of realisation of the interface
 *
 * */


matrix make_matrix(int n_rows, int n_columns){
  matrix result;

  double **array = (double**) calloc(n_rows, sizeof(*array));
  if(array == NULL) NOT_ENOUGH_MEMORY_EXIT;
  for(int i = 0; i < n_rows; i++){
    array[i] = (double*) calloc(n_columns, sizeof(*array[i]));
    if(array[i] != NULL){
      memset(array[i], 0.0, n_columns * sizeof(*array[i]));
    }
    else{
			NOT_ENOUGH_MEMORY_EXIT;
    }
  }

  result.n_columns = n_columns;
  result.n_rows = n_rows;
  result.array = array;
	result.n_permutations = 0;

  return result;
}


int show_matrix(IN const matrix *in_matrix){
  return walk_on_matrix(in_matrix, UNUSED_PARAM, show, UNUSED_PARAM);
}


int init_matrix(MODYFIED matrix *out_matrix){
  return walk_on_matrix(UNUSED_PARAM, out_matrix, init, UNUSED_PARAM);
}


int init_matrix_by_random(MODYFIED matrix *out_matrix, int32_t down, int32_t up){ 
	int64_t tmp = ((int64_t)up << LENGHT_OF_WORD/2) + down;
	double param = (double)(tmp);
  return walk_on_matrix(UNUSED_PARAM, out_matrix, init_by_random, param);
}


int init_matrix_as_unit(MODYFIED matrix *out_matrix){
  return walk_on_matrix(UNUSED_PARAM, out_matrix, init_as_unit, UNUSED_PARAM);
}


int init_matrix_by_function(MODYFIED matrix *in_matrix, init_user foo){
	if(matrix_exists(in_matrix)){
    for(int i = 0; i < in_matrix->n_rows; i++){
      for(int j = 0; j < in_matrix->n_columns; j++){
				in_matrix->array[i][j] = foo(i, j);
      }
    }
    return true;
  }
  else;

	return false;
}


int transponent(IN const matrix *in_matrix, OUT matrix *out_matrix){
  int status = true;
  matrix result = make_matrix(in_matrix->n_columns, in_matrix->n_rows);
  status = status && walk_on_matrix(in_matrix, &result, transpon, UNUSED_PARAM);
  status = status && copy_matrix(&result, out_matrix);
  delete_matrix(&result);
  return status;
}


int multiplex_matrixes(IN const matrix *in_matrix_1, IN const matrix *in_matrix_2, OUT matrix *out_matrix){
  if(in_matrix_2->n_rows == in_matrix_1->n_columns){

    matrix result = make_matrix(in_matrix_1->n_rows, in_matrix_2->n_columns);

    for(int i = 0; i < result.n_rows ;i++){
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
  else{
    return false;
  }
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
		printf("permutations: %d\n", triangle.n_permutations);
    delete_matrix(&triangle);
    return result;
  }
  else{
    puts("determinant: " GENERAL_ERR);
    return 0;
  }
}


int rank(IN const matrix *in_matrix){
	matrix tmp = make_matrix(0,0);
	copy_matrix(in_matrix, &tmp);
	triangle_form(&tmp, &tmp);
	int result = tmp.n_rows;
	for(int i = 0; i < tmp.n_rows; ++i){
		for(int j = tmp.n_columns-1; i >= 0; --j){
			if(tmp.array[i][j] != 0) break;
			if(j == 0) --result;
		}
	}
	delete_matrix(&tmp);
	return result;
}


int copy_matrix(IN const matrix *in_matrix, OUT matrix *out_matrix){
  if(in_matrix != out_matrix
     && matrix_exists(out_matrix)
     && matrix_exists(in_matrix))
  {
    int status = true;
    delete_matrix(out_matrix);
    *out_matrix = make_matrix(in_matrix->n_rows, in_matrix->n_columns);
    status = status && walk_on_matrix(in_matrix, out_matrix, copy_element, UNUSED_PARAM);
		out_matrix->n_permutations = in_matrix->n_permutations;
    return status;
  }
  else;

	return false;
}


int delete_matrix(matrix *in_matrix){
	if(matrix_exists(in_matrix)){
		for(int i = 0; i < in_matrix->n_rows; i++){
			if(in_matrix->array){
				free(in_matrix->array[i]);
				in_matrix->array[i] = NULL;
			}
		}
		free(in_matrix->array);
		in_matrix->array = NULL;
		return true;
	}
	else;

	return false;
}


int rows_swap(int i_row_1, int i_row_2, MODYFIED matrix *in_matrix){
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
  else;

	return false;
}


int columns_swap(int i_col_1, int i_col_2, MODYFIED matrix *in_matrix){
  int status = true;
  status = status && transponent(in_matrix, in_matrix);
  status = status && rows_swap(i_col_1, i_col_2, in_matrix);
  status = status && transponent(in_matrix, in_matrix);
  return status;
}


int triangle_form(IN const matrix *in_matrix, OUT matrix *out_matrix){
  return triangle_form_of_augmented_matrix(in_matrix, out_matrix, NULL);
}


int row_mult_on_const(double factor, int i_row, MODYFIED matrix *out_matrix){
  if(indexes_are_right(i_row, UNUSED_PARAM, out_matrix->n_rows) && matrix_exists(out_matrix)){
    int status = true;
    matrix tmp = make_matrix(1, out_matrix->n_columns);
    free(tmp.array[0]);
    tmp.array[0] = out_matrix->array[i_row];
    status = status && walk_on_matrix(UNUSED_PARAM, &tmp, mult, factor);
    free(tmp.array);
    return status;
  }
  else;

	return false;
}


int column_mult_on_const(double factor, int i_column, MODYFIED matrix *out_matrix){
  int status = true;
  status = status && transponent(out_matrix, out_matrix);
  status = status && row_mult_on_const(factor, i_column, out_matrix);
  status = status && transponent(out_matrix, out_matrix);
  return status;
}


int rows_sub(
  double factor,
  int i_subtracted_row,
  int i_subtracting_row,
  MODYFIED matrix *out_matrix)
{
  if(indexes_are_right(i_subtracted_row, i_subtracting_row, out_matrix->n_rows) && matrix_exists(out_matrix)){
    for(int i = 0; i < out_matrix->n_columns; i++){
      out_matrix->array[i_subtracted_row][i] -= out_matrix->array[i_subtracting_row][i] * factor;
    }
    return true;
  }
  else;

	return false;
}


int columns_sub(
  double factor,
  int i_subtracted_column,
  int i_subtracting_column,
  MODYFIED matrix *out_matrix)
{
  int status = true;
  status = status && transponent(out_matrix, out_matrix);
  status = status && rows_sub(factor, i_subtracted_column, i_subtracting_column, out_matrix);
  status = status && transponent(out_matrix, out_matrix);
  return status;
}


int copy_row_to_other_matrix(
  int i_source,
  int i_reciever,
  IN const matrix *in_matrix,
  OUT matrix *out_matrix )
{
  if(matrix_exists(in_matrix) 
    && matrix_exists(out_matrix)
    && indexes_are_right(i_source, UNUSED_PARAM, in_matrix->n_rows)
    && indexes_are_right(i_reciever, UNUSED_PARAM, out_matrix->n_rows)
    && in_matrix->n_columns == out_matrix->n_columns)
  {
    for(int i = 0; i < in_matrix->n_columns; i++){
      out_matrix->array[i_reciever][i] = in_matrix->array[i_source][i];
    }
    return true;
  }
  else;

	return false;
}


int copy_column_to_other_matrix(
  int i_source,
  int i_reciever,
  IN const matrix *in_matrix,
  OUT matrix *out_matrix )
{
  int status = true;
	matrix tmp = make_matrix(0,0);
  status = status && transponent(in_matrix, &tmp);
  status = status && transponent(out_matrix, out_matrix);
  status = status && copy_row_to_other_matrix(i_source, i_reciever, in_matrix, out_matrix);
  status = status && transponent(out_matrix, out_matrix);
	status = status && delete_matrix(&tmp);
  return status;
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

    status = status && triangle_form_of_augmented_matrix(&tmp, &tmp, &result);
    
    double factor = 0;
    for(int i = tmp.n_rows-1; i > 0; i--){
      factor = tmp.array[i][i];
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
  else;

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


int triangle_form_of_augmented_matrix(IN const matrix *in_matrix, OUT matrix *out_matrix, OUT matrix *half_an_inverted_matrix){
	int status = true;
  matrix result = make_matrix(0,0);
  matrix result_2 = make_matrix(in_matrix->n_rows,in_matrix->n_columns);
  status = status && copy_matrix(in_matrix, &result);
  status = status && init_matrix_as_unit(&result_2);

  double tmp = 0;
  for(int i = 0; i < result.n_columns; i++){

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
        tmp = result.array[j][i];
        status = status && row_mult_on_const(result.array[i][i], j, &result);
        status = status && row_mult_on_const(result.array[i][i], j, &result_2);
        status = status && rows_sub(tmp, j, i, &result);
        status = status && rows_sub(tmp, j, i, &result_2);
        status = status && row_mult_on_const(1/result.array[i][i], j, &result);
        status = status && row_mult_on_const(1/result.array[i][i], j, &result_2);
      }
      else{
        continue;
      }
    }
  }

  status = status && copy_matrix(&result, out_matrix);
  status = status && copy_matrix(&result_2, half_an_inverted_matrix);
  status = status && delete_matrix(&result);
  status = status && delete_matrix(&result_2);
  return status;
}


int walk_on_matrix(
	const IN matrix *in_matrix,
	OUT matrix *out_matrix,
	action foo,
	double param)
{
	int n_rows = 0;
	int n_columns = 0;
  if(matrix_exists(in_matrix) || matrix_exists(out_matrix)){
		n_rows = matrix_exists(in_matrix) ? in_matrix->n_rows : out_matrix->n_rows;
		n_columns = matrix_exists(in_matrix) ? in_matrix->n_columns : out_matrix->n_columns;
    for(int i = 0; i < n_rows; i++){
      for(int j = 0; j < n_columns; j++){
        foo(i, j, in_matrix, out_matrix, param);
      }
    }
    return true;
  }
  else{
    return false;
  }
}


void init_by_random(
	int i_row,
	int i_column,
	UNUSED const matrix *in_matrix,
	OUT matrix *out_matrix,
	double param)
{
	UNUSED_F(in_matrix);
	srand((int)time(NULL)+(rand()));
	int32_t up = (int32_t)((((int64_t)param) & FIRST_HALF_OF_WORD) >> LENGHT_OF_WORD/2);
	int32_t down = (int32_t)((int64_t)param & SECOND_HALF_OF_WORD);
  out_matrix->array[i_row][i_column] = ((rand() % (up-down+1)) + down);
}


void show(
	int i_row,
	int i_column,
	IN const matrix *in_matrix,
	UNUSED matrix *out_matrix,
	UNUSED double param)
{
	UNUSED_F(out_matrix);
	UNUSED_F(param);
  printf(" %.2f ", in_matrix->array[i_row][i_column]);
  if(i_column == in_matrix->n_columns-1) puts("");
}


void init(
	int i_row,
	int i_column,
	UNUSED const matrix *in_matrix,
	OUT matrix *out_matrix,
	UNUSED double param)
{
	UNUSED_F(in_matrix);
	UNUSED_F(param);
  printf("Enter %d %d elment: ",i_row, i_column);
  scanf("%lf", &out_matrix->array[i_row][i_column]);
}


void init_as_unit(
	int i_row,
	int i_column,
	UNUSED const matrix *in_matrix,
	OUT matrix *out_matrix,
	UNUSED double param)
{
	UNUSED_F(in_matrix);
	UNUSED_F(param);
  if(i_row == i_column) out_matrix->array[i_row][i_column] = 1;
}


void mult(
	int i_row,
	int i_column,
	UNUSED const matrix *in_matrix,
	OUT matrix *out_matrix,
	UNUSED double param)
{
	UNUSED_F(in_matrix);
	UNUSED_F(param);
  out_matrix->array[i_row][i_column] *= param;
}


void copy_element(
	int i_row,
	int i_column,
	IN const matrix *in_matrix,
	OUT matrix *out_matrix,
	UNUSED double param)
{
	UNUSED_F(param);
  out_matrix->array[i_row][i_column] = in_matrix->array[i_row][i_column];
}


void transpon(
	int i_row,
	int i_column,
	IN const matrix *in_matrix,
	OUT matrix *out_matrix,
	UNUSED double param)
{
	UNUSED_F(param);
  out_matrix->array[i_column][i_row] = in_matrix->array[i_row][i_column];
}



#include <stdio.h>
#include "matrix.h"
#include <stdlib.h>
#include <time.h>


double foo(int i, int j){
	return i + 5*j;
}


int main(){
  unsigned int n_rows, n_columns;
  n_rows = n_columns = 0;

  puts("Enter number of lines and columns of matrix of equations");
  scanf("%u%u", &n_rows, &n_columns);
  matrix equations = make_matrix(n_rows, n_columns);
	srand((int)time(NULL));
  init_matrix_by_random(&equations);

  puts("Enter vector of results");
  matrix results = make_matrix(n_rows, 1);
  init_matrix(&results);
  show_matrix(&equations);
  puts("-------------------------------------");

  if(inverse_matrix(&equations, &equations)){
    puts("inversed");
  }else{
    puts("no inversible");
  }
  show_matrix(&equations);
  puts("-------------------------------------");

  double det = determinant(&equations);
  printf("\n det = %.2f\n", det);

  if(det != 0){
    matrix buf = make_matrix(n_rows, 1);

    double det_x;

    for(int i = 0; i < equations.n_columns; i++){
      copy_column_to_other_matrix(i, 0, &equations, &buf);
      copy_column_to_other_matrix(0, i, &results, &equations);

      det_x = determinant(&equations);
      printf("x%i = %.2f\n", i, det_x/det);

      copy_column_to_other_matrix(0, i, &buf, &equations);
    }
    delete_matrix(&buf);
  }
  else{
    puts("equations have infinity roots or don't have it");
  }
  delete_matrix(&equations);
  delete_matrix(&results);

  return 0;
}

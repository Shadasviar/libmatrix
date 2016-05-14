#include <stdio.h>
#include "matrix.h"
#include <stdlib.h>
#include <time.h>

int main(){
     int a, b;
    a = b = 0;

    puts("Enter size of matrix");
    scanf("%d%d", &a, &b);

    matrix matr = make_matrix(a, b);
    init_matrix(&matr);

		puts("DEBUG:");
		matrix tmp = make_matrix(a,b);
		show_matrix(&tmp);
		copy_column_to_other_matrix(1,1,&matr, &tmp);
		show_matrix(&tmp);
		puts("DEBUG:");
	
		printf("Determinant: %f\n", determinant(&matr));
		printf("Rank: %d\n", rank(&matr));
		matrix pp = make_matrix(0, 0);
		triangle_form(&matr, &pp);
		show_matrix(&pp);
		

    puts("Enter vector of results");
    matrix results = make_matrix(a, 1);
    init_matrix_by_random(&results, 1, 4);

    puts("Matrix:");
    show_matrix(&matr);
    puts("Results:");
    show_matrix(&results);

    matrix res = make_matrix(a, 1);
    if(inverse_matrix(&matr, &matr)){
        multiplex_matrices(&matr, &results, &res);
        puts("Results:");
        show_matrix(&res);
    }
    else{
        puts("no roots, sorry");

        matrix pseudo = make_matrix(0,0);
        matrix tmp = make_matrix(0,0);
        transpose(&matr, &tmp);
        multiplex_matrices(&tmp, &matr, &pseudo);
        inverse_matrix(&pseudo, &pseudo);
        multiplex_matrices(&pseudo, &tmp, &pseudo);
        multiplex_matrices(&pseudo, &results, &res);

        puts("Optimal roots are:");
        show_matrix(&res);
				delete_matrix(&pseudo);
				delete_matrix(&tmp);
    }

		delete_matrix(&matr);
		delete_matrix(&results);
		delete_matrix(&res);
    return 0;
}

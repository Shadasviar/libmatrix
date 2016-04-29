#include <stdio.h>
#include <matrix.h>

int main(){
    int a, b;
    a = b = 0;

    puts("Enter size of matrix");
    scanf("%d%d", &a, &b);

    matrix matr = make_matrix(a, b);
    init_matrix(&matr);

    puts("Enter vector of results");
    matrix results = make_matrix(a, 1);
    init_matrix(&results);

    puts("Matrix:");
    show_matrix(&matr);
    puts("Results:");
    show_matrix(&results);

    matrix res = make_matrix(a, 1);
    if(inverse_matrix(&matr, &matr)){
        multiplex_matrixes(&matr, &results, &res);
        puts("Results:");
        show_matrix(&res);
    }
    else{
        puts("no roots, sorry");

        matrix pseudo = make_matrix(0,0);
        matrix tmp = make_matrix(0,0);
        transponent(&matr, &tmp);
        multiplex_matrixes(&tmp, &matr, &pseudo);
        inverse_matrix(&pseudo, &pseudo);
        multiplex_matrixes(&pseudo, &tmp, &pseudo);
        multiplex_matrixes(&pseudo, &results, &res);

        puts("Optimal roots are:");
        show_matrix(&res);
    }


    return 0;
}

main.o: main.c
	gcc -c -o main.o main.c -Wall -Wextra -std=c11

matrix.o: matrix.c matrix.h
	gcc -c -o matrix.o matrix.c -std=c11 -Wall -Wextra

libmatrix.a: matrix.o
	ar cr libmatrix.a matrix.o

matrix: main.o libmatrix.a
	gcc -o matrix main.o -L. -l:libmatrix.a -lm -std=c11 -Wextra

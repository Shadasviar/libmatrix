main.o: main.c matrix.h
	gcc -c -o main.o main.c -Wall -Wextra -std=c11

matrix.o: matrix.c matrix.h
	gcc -c -o matrix.o matrix.c -std=c11 -Wall -Wextra

matrix: main.o matrix.o
	gcc -o matrix main.o matrix.o -std=c11 -Wextra
